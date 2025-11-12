#include "db/Database.h"
#include <sstream>
#include <vector>
#include <cstring>
#include <iostream>

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
        SQLHSTMT hstmt_local = SQL_NULL_HSTMT;
        if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_local) != SQL_SUCCESS) {
            err = "Failed to allocate statement handle";
            return false;
        }
        SQLRETURN rc = SQLExecDirectA(hstmt_local, (SQLCHAR*)sql.c_str(), (SQLINTEGER)SQL_NTS);
        if (!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
            err = extractDiag(SQL_HANDLE_STMT, hstmt_local);
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_local);
            return false;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt_local);
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
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = SQL_NULL_HENV;
        return false;
    }

    if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) != SQL_SUCCESS) {
        lastErrorMessage = "Failed to allocate ODBC connection handle";
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = SQL_NULL_HENV;
        return false;
    }

    SQLCHAR outConn[1024];
    SQLSMALLINT outLen = 0;

    SQLRETURN rc = SQLDriverConnectA(
        hdbc,
        NULL,
        (SQLCHAR*)connectionString.c_str(),
        (SQLSMALLINT)connectionString.length(),
        outConn,
        sizeof(outConn),
        &outLen,
        SQL_DRIVER_NOPROMPT
    );

    if (!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
        lastErrorMessage = buildErrorMessage(SQL_HANDLE_DBC, hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        hdbc = SQL_NULL_HDBC;
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = SQL_NULL_HENV;
        return false;
    }

    connected = true;
    std::cout << "Database connected successfully!" << std::endl;
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
    if (!connected) {
        lastErrorMessage = "Not connected to database";
        return false;
    }

    std::string err;

    const char* createDatabase =
        "IF NOT EXISTS(SELECT name FROM master.dbo.sysdatabases WHERE name = 'MyTetris') "
        "CREATE DATABASE MyTetris;";

    const char* useDatabase = "USE MyTetris;";

    const char* createPlayers =
        "IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Players]') AND type = N'U') "
        "CREATE TABLE dbo.Players ("
        "  Id INT IDENTITY(1,1) PRIMARY KEY,"
        "  Name NVARCHAR(100) NOT NULL UNIQUE,"
        "  CreatedAt DATETIME2 NOT NULL DEFAULT GETDATE()"
        ");";

    const char* createScores =
        "IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Scores]') AND type = N'U') "
        "CREATE TABLE dbo.Scores ("
        "  Id INT IDENTITY(1,1) PRIMARY KEY,"
        "  PlayerId INT NOT NULL FOREIGN KEY REFERENCES dbo.Players(Id),"
        "  Score INT NOT NULL,"
        "  TotalLines INT NOT NULL,"
        "  Level INT NOT NULL,"
        "  DurationSeconds INT NOT NULL,"
        "  CreatedAt DATETIME2 NOT NULL DEFAULT GETDATE()"
        ");";

    const char* createStats =
        "IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GameStats]') AND type = N'U') "
        "CREATE TABLE dbo.GameStats ("
        "  Id INT IDENTITY(1,1) PRIMARY KEY,"
        "  PlayerId INT NOT NULL FOREIGN KEY REFERENCES dbo.Players(Id),"
        "  Score INT NOT NULL,"
        "  DurationSeconds INT NOT NULL,"
        "  Level INT NOT NULL,"
        "  CountI INT NOT NULL DEFAULT 0,"
        "  CountO INT NOT NULL DEFAULT 0,"
        "  CountT INT NOT NULL DEFAULT 0,"
        "  CountS INT NOT NULL DEFAULT 0,"
        "  CountZ INT NOT NULL DEFAULT 0,"
        "  CountJ INT NOT NULL DEFAULT 0,"
        "  CountL INT NOT NULL DEFAULT 0,"
        "  TotalLines INT NOT NULL,"
        "  CreatedAt DATETIME2 NOT NULL DEFAULT GETDATE()"
        ");";

    // Создаем базу данных если не существует
    if (!execDirect(hdbc, createDatabase, err)) {
        std::cout << "Note: Could not create database (might already exist): " << err << std::endl;
    }

    // Используем нашу базу данных
    if (!execDirect(hdbc, useDatabase, err)) {
        lastErrorMessage = "Failed to use MyTetris database: " + err;
        return false;
    }

    // Создаем таблицы
    bool success = execDirect(hdbc, createPlayers, err)
        && execDirect(hdbc, createScores, err)
        && execDirect(hdbc, createStats, err);

    if (success) {
        std::cout << "Database schema ensured successfully!" << std::endl;
    }
    else {
        lastErrorMessage = "Failed to ensure schema: " + err;
    }

    return success;
}

std::optional<int> Database::ensurePlayer(const std::string& playerName) {
    if (!connected) return std::nullopt;

    // Try get existing
    SQLHSTMT hstmt1 = SQL_NULL_HSTMT;
    if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt1) != SQL_SUCCESS) {
        return std::nullopt;
    }

    std::ostringstream query1;
    query1 << "SELECT Id FROM dbo.Players WHERE Name = N'" << escapeQuotes(playerName) << "'";

    SQLRETURN rc1 = SQLExecDirectA(hstmt1, (SQLCHAR*)query1.str().c_str(), (SQLINTEGER)SQL_NTS);
    if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER playerId = 0;
        SQLLEN idLen = 0;

        SQLBindCol(hstmt1, 1, SQL_C_SLONG, &playerId, 0, &idLen);

        if (SQLFetch(hstmt1) == SQL_SUCCESS && idLen != SQL_NULL_DATA) {
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
            return static_cast<int>(playerId);
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

    // Insert new
    SQLHSTMT hstmt2 = SQL_NULL_HSTMT;
    if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt2) != SQL_SUCCESS) {
        return std::nullopt;
    }

    std::ostringstream query2;
    query2 << "INSERT INTO dbo.Players(Name) VALUES (N'" << escapeQuotes(playerName) << "')";

    SQLRETURN rc2 = SQLExecDirectA(hstmt2, (SQLCHAR*)query2.str().c_str(), (SQLINTEGER)SQL_NTS);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt2);

    if (!(rc2 == SQL_SUCCESS || rc2 == SQL_SUCCESS_WITH_INFO)) {
        return std::nullopt;
    }

    // Read newly created id
    SQLHSTMT hstmt3 = SQL_NULL_HSTMT;
    if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt3) != SQL_SUCCESS) return std::nullopt;

    std::ostringstream query3;
    query3 << "SELECT Id FROM dbo.Players WHERE Name = N'" << escapeQuotes(playerName) << "'";

    SQLRETURN rc3 = SQLExecDirectA(hstmt3, (SQLCHAR*)query3.str().c_str(), (SQLINTEGER)SQL_NTS);
    if (rc3 == SQL_SUCCESS || rc3 == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER newPlayerId = 0;
        SQLLEN newIdLen = 0;

        SQLBindCol(hstmt3, 1, SQL_C_SLONG, &newPlayerId, 0, &newIdLen);

        if (SQLFetch(hstmt3) == SQL_SUCCESS && newIdLen != SQL_NULL_DATA) {
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt3);
            return static_cast<int>(newPlayerId);
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hstmt3);
    return std::nullopt;
}

bool Database::insertScore(int playerId, int score, int totalLines, int level, int durationSeconds) {
    if (!connected) return false;
    SQLHSTMT hstmt_local = SQL_NULL_HSTMT;
    if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_local) != SQL_SUCCESS) return false;

    std::ostringstream oss;
    oss << "INSERT INTO dbo.Scores(PlayerId, Score, TotalLines, Level, DurationSeconds) VALUES ("
        << playerId << "," << score << "," << totalLines << "," << level << "," << durationSeconds << ")";
    SQLRETURN rc = SQLExecDirectA(hstmt_local, (SQLCHAR*)oss.str().c_str(), (SQLINTEGER)SQL_NTS);
    bool ok = (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_local);
    return ok;
}

std::vector<std::pair<std::string, int>> Database::fetchTopScores(int topN) {
    std::vector<std::pair<std::string, int>> rows;
    if (!connected) return rows;

    SQLHSTMT hstmt_local = SQL_NULL_HSTMT;
    if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_local) != SQL_SUCCESS) return rows;

    std::ostringstream oss;
    oss << "SELECT TOP (" << topN << ") p.Name, s.Score "
        << "FROM dbo.Scores s JOIN dbo.Players p ON p.Id = s.PlayerId "
        << "ORDER BY s.Score DESC, s.CreatedAt ASC";

    SQLRETURN rc = SQLExecDirectA(hstmt_local, (SQLCHAR*)oss.str().c_str(), (SQLINTEGER)SQL_NTS);
    if (!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt_local);
        return rows;
    }

    // Используем SQLBindCol для безопасного получения данных
    char nameBuf[128] = { 0 }; // Инициализируем нулями
    SQLINTEGER scoreVal = 0;
    SQLLEN nameLen = 0, scoreLen = 0;

    // Привязываем колонки
    SQLBindCol(hstmt_local, 1, SQL_C_CHAR, nameBuf, sizeof(nameBuf), &nameLen);
    SQLBindCol(hstmt_local, 2, SQL_C_SLONG, &scoreVal, 0, &scoreLen);

    while (SQLFetch(hstmt_local) == SQL_SUCCESS) {
        // Гарантируем нуль-терминацию
        if (nameLen == SQL_NULL_DATA || nameLen <= 0) {
            nameBuf[0] = '\0';
        }
        else {
            size_t safeLen = static_cast<size_t>(std::min(nameLen, static_cast<SQLLEN>(sizeof(nameBuf) - 1)));
            nameBuf[safeLen] = '\0';
        }

        // Если счет NULL, используем 0
        if (scoreLen == SQL_NULL_DATA) {
            scoreVal = 0;
        }

        rows.emplace_back(std::string(nameBuf), static_cast<int>(scoreVal));
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_local);
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
    SQLHSTMT hstmt_local = SQL_NULL_HSTMT;
    if (SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_local) != SQL_SUCCESS) return false;

    std::ostringstream oss;
    oss << "INSERT INTO dbo.GameStats("
        << "PlayerId, Score, DurationSeconds, Level, "
        << "CountI, CountO, CountT, CountS, CountZ, CountJ, CountL, TotalLines) VALUES ("
        << playerId << "," << score << "," << durationSeconds << "," << level << ","
        << pieceCounts[0] << "," << pieceCounts[1] << "," << pieceCounts[2] << "," << pieceCounts[3] << ","
        << pieceCounts[4] << "," << pieceCounts[5] << "," << pieceCounts[6] << "," << totalLines << ")";

    SQLRETURN rc = SQLExecDirectA(hstmt_local, (SQLCHAR*)oss.str().c_str(), (SQLINTEGER)SQL_NTS);
    bool ok = (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_local);
    return ok;
}