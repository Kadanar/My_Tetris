#include "db/Database.h"
#include <sstream>
#include <vector>
#include <cstring>

namespace {
	static std::string extractDiag(SQLSMALLINT handleType, SQLHANDLE handle) {
		SQLSMALLINT i = 1;
		SQLINTEGER native;
		SQLCHAR state[6];
		SQLCHAR text[512];
		SQLSMALLINT len;
		std::ostringstream oss;
		while (SQLGetDiagRecA(handleType, handle, i, state, &native, text, sizeof(text), &len) == SQL_SUCCESS) {
			oss << "[" << state << "] (" << native << ") " << text << " ";
			++i;
		}
		return oss.str();
	}

	static bool execDirect(SQLHDBC hdbc, const std::string& sql, std::string& err) {
		SQLHSTMT hstmt = SQL_NULL_HSTMT;
		if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS) {
			err = "Failed to allocate statement handle";
			return false;
		}
		SQLRETURN rc = SQLExecDirectA(hstmt, (SQLCHAR*)sql.c_str(), (SQLINTEGER)SQL_NTS);
		if (!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
			err = extractDiag(SQL_HANDLE_STMT, hstmt);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			return false;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		return true;
	}

	static std::string escapeQuotes(const std::string& s) {
		std::string r;
		r.reserve(s.size());
		for (char c : s) {
			if (c == '\'') r += "''"; else r += c;
		}
		return r;
	}
}

Database::~Database() {
	disconnect();
}

std::string Database::buildErrorMessage(SQLSMALLINT handleType, SQLHANDLE handle) const {
	return extractDiag(handleType, handle);
}

bool Database::connect(const std::string& connectionString) {
	disconnect();

	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv) != SQL_SUCCESS) {
		lastErrorMessage = "Failed to allocate ODBC environment handle";
		return false;
	}

	if (SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0) != SQL_SUCCESS) {
		lastErrorMessage = "Failed to set ODBC environment attributes";
		return false;
	}

	if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) != SQL_SUCCESS) {
		lastErrorMessage = "Failed to allocate ODBC connection handle";
		return false;
	}

	SQLCHAR outConn[1] = {0};
	SQLSMALLINT outLen = 0;
	SQLRETURN rc = SQLDriverConnectA(
		hdbc,
		(SQLHWND)NULL,
		(SQLCHAR*)connectionString.c_str(),
		(SQLSMALLINT)SQL_NTS,
		outConn,
		(SQLSMALLINT)0,
		&outLen,
		(SQLUSMALLINT)SQL_DRIVER_NOPROMPT
	);

	if (!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
		lastErrorMessage = buildErrorMessage(SQL_HANDLE_DBC, hdbc);
		return false;
	}

	connected = true;
	return true;
}

void Database::disconnect() {
	if (hdbc != SQL_NULL_HDBC) {
		SQLDisconnect(hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		hdbc = SQL_NULL_HDBC;
	}
	if (henv != SQL_NULL_HENV) {
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
		henv = SQL_NULL_HENV;
	}
	connected = false;
}

bool Database::ensureSchema() {
	if (!connected) return false;

	std::string err;

	const char* createPlayers =
		"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Players]') AND type = N'U') "
		"BEGIN "
		"CREATE TABLE dbo.Players ("
		"  Id INT IDENTITY(1,1) PRIMARY KEY,"
		"  Name NVARCHAR(100) NOT NULL UNIQUE"
		");"
		"END;";

	const char* createScores =
		"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Scores]') AND type = N'U') "
		"BEGIN "
		"CREATE TABLE dbo.Scores ("
		"  Id INT IDENTITY(1,1) PRIMARY KEY,"
		"  PlayerId INT NOT NULL FOREIGN KEY REFERENCES dbo.Players(Id),"
		"  Score INT NOT NULL,"
		"  TotalLines INT NOT NULL,"
		"  Level INT NOT NULL,"
		"  DurationSeconds INT NOT NULL,"
		"  CreatedAt DATETIME2 NOT NULL DEFAULT SYSUTCDATETIME()"
		");"
		"END;";

	const char* createStats =
		"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GameStats]') AND type = N'U') "
		"BEGIN "
		"CREATE TABLE dbo.GameStats ("
		"  Id INT IDENTITY(1,1) PRIMARY KEY,"
		"  PlayerId INT NOT NULL FOREIGN KEY REFERENCES dbo.Players(Id),"
		"  Score INT NOT NULL,"
		"  DurationSeconds INT NOT NULL,"
		"  Level INT NOT NULL,"
		"  CountI INT NOT NULL,"
		"  CountO INT NOT NULL,"
		"  CountT INT NOT NULL,"
		"  CountS INT NOT NULL,"
		"  CountZ INT NOT NULL,"
		"  CountJ INT NOT NULL,"
		"  CountL INT NOT NULL,"
		"  TotalLines INT NOT NULL,"
		"  CreatedAt DATETIME2 NOT NULL DEFAULT SYSUTCDATETIME()"
		");"
		"END;";

	return execDirect(hdbc, createPlayers, err)
		&& execDirect(hdbc, createScores, err)
		&& execDirect(hdbc, createStats, err);
}

std::optional<int> Database::ensurePlayer(const std::string& playerName) {
	if (!connected) return std::nullopt;

	// Try get existing
	{
		SQLHSTMT hstmt = SQL_NULL_HSTMT;
		if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS) {
			return std::nullopt;
		}
		std::ostringstream q;
		q << "SELECT Id FROM dbo.Players WHERE Name = N'" << escapeQuotes(playerName) << "'";
		if (!(SQLExecDirectA(hstmt, (SQLCHAR*)q.str().c_str(), (SQLINTEGER)SQL_NTS) == SQL_SUCCESS ||
			SQLExecDirectA(hstmt, (SQLCHAR*)q.str().c_str(), (SQLINTEGER)SQL_NTS) == SQL_SUCCESS_WITH_INFO)) {
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			return std::nullopt;
		}
		SQLINTEGER id = 0;
		if (SQLFetch(hstmt) == SQL_SUCCESS) {
			SQLGetData(hstmt, 1, SQL_C_SLONG, &id, (SQLLEN)0, (SQLLEN*)NULL);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			return static_cast<int>(id);
		}
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	}

	// Insert new
	{
		SQLHSTMT hstmt = SQL_NULL_HSTMT;
		if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS) {
			return std::nullopt;
		}
		std::ostringstream ins;
		ins << "INSERT INTO dbo.Players(Name) VALUES (N'" << escapeQuotes(playerName) << "')";
		if (!(SQLExecDirectA(hstmt, (SQLCHAR*)ins.str().c_str(), (SQLINTEGER)SQL_NTS) == SQL_SUCCESS ||
			SQLExecDirectA(hstmt, (SQLCHAR*)ins.str().c_str(), (SQLINTEGER)SQL_NTS) == SQL_SUCCESS_WITH_INFO)) {
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			return std::nullopt;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	}

	// Read newly created id
	{
		SQLHSTMT s2 = SQL_NULL_HSTMT;
		if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &s2) != SQL_SUCCESS) return std::nullopt;
		std::ostringstream q;
		q << "SELECT Id FROM dbo.Players WHERE Name = N'" << escapeQuotes(playerName) << "'";
		if (!(SQLExecDirectA(s2, (SQLCHAR*)q.str().c_str(), (SQLINTEGER)SQL_NTS) == SQL_SUCCESS ||
			SQLExecDirectA(s2, (SQLCHAR*)q.str().c_str(), (SQLINTEGER)SQL_NTS) == SQL_SUCCESS_WITH_INFO)) {
			SQLFreeHandle(SQL_HANDLE_STMT, s2);
			return std::nullopt;
		}
		SQLINTEGER id = 0;
		if (SQLFetch(s2) == SQL_SUCCESS) {
			SQLGetData(s2, 1, SQL_C_SLONG, &id, (SQLLEN)0, (SQLLEN*)NULL);
			SQLFreeHandle(SQL_HANDLE_STMT, s2);
			return static_cast<int>(id);
		}
		SQLFreeHandle(SQL_HANDLE_STMT, s2);
	}
	return std::nullopt;
}

bool Database::insertScore(int playerId, int score, int totalLines, int level, int durationSeconds) {
	if (!connected) return false;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS) return false;

	std::ostringstream oss;
	oss << "INSERT INTO dbo.Scores(PlayerId, Score, TotalLines, Level, DurationSeconds) VALUES ("
		<< playerId << "," << score << "," << totalLines << "," << level << "," << durationSeconds << ")";
	SQLRETURN rc = SQLExecDirectA(hstmt, (SQLCHAR*)oss.str().c_str(), (SQLINTEGER)SQL_NTS);
	bool ok = (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return ok;
}

std::vector<std::pair<std::string, int>> Database::fetchTopScores(int topN) {
	std::vector<std::pair<std::string, int>> rows;
	if (!connected) return rows;

	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS) return rows;

	std::ostringstream oss;
	oss << "SELECT TOP (" << topN << ") p.Name, s.Score "
		<< "FROM dbo.Scores s JOIN dbo.Players p ON p.Id = s.PlayerId "
		<< "ORDER BY s.Score DESC, s.CreatedAt ASC";

	SQLRETURN rc = SQLExecDirectA(hstmt, (SQLCHAR*)oss.str().c_str(), (SQLINTEGER)SQL_NTS);
	if (!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		return rows;
	}

	char nameBuf[128];
	SQLINTEGER scoreVal = 0;

	while (SQLFetch(hstmt) == SQL_SUCCESS) {
		SQLLEN outLen = 0;
		std::memset(nameBuf, 0, sizeof(nameBuf));
		SQLGetData(hstmt, 1, SQL_C_CHAR, nameBuf, (SQLLEN)sizeof(nameBuf), &outLen);
		SQLGetData(hstmt, 2, SQL_C_SLONG, &scoreVal, (SQLLEN)0, (SQLLEN*)NULL);
		rows.emplace_back(std::string(nameBuf), static_cast<int>(scoreVal));
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return rows;
}

bool Database::insertGameStats(
	int playerId,
	int score,
	int durationSeconds,
	int level,
	const int pieceCounts[7],
	int totalLines
) {
	if (!connected) return false;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS) return false;

	std::ostringstream oss;
	oss << "INSERT INTO dbo.GameStats("
		"PlayerId, Score, DurationSeconds, Level, "
		"CountI, CountO, CountT, CountS, CountZ, CountJ, CountL, TotalLines) VALUES ("
		<< playerId << "," << score << "," << durationSeconds << "," << level << ","
		<< pieceCounts[0] << "," << pieceCounts[1] << "," << pieceCounts[2] << "," << pieceCounts[3] << ","
		<< pieceCounts[4] << "," << pieceCounts[5] << "," << pieceCounts[6] << "," << totalLines << ")";

	SQLRETURN rc = SQLExecDirectA(hstmt, (SQLCHAR*)oss.str().c_str(), (SQLINTEGER)SQL_NTS);
	bool ok = (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return ok;
}

