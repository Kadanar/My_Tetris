#pragma once
#include <string>
#include <vector>
#include <utility>
#include <optional>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef ODBCVER
#define ODBCVER 0x0380
#endif
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

class Database {
private:
    SQLHENV henv = SQL_NULL_HENV;
    SQLHDBC hdbc = SQL_NULL_HDBC;
    bool connected = false;
    std::string lastErrorMessage;

    std::string buildErrorMessage(SQLSMALLINT handleType, SQLHANDLE handle) const;

public:
    Database() = default;
    ~Database();

    bool connect(const std::string& connectionString);
    void disconnect();ûnected; }
    const std::string& getLastError() const { return lastErrorMessage; }

    bool ensureSchema();

    std::optional<int> ensurePlayer(const std::string& playerName);

    bool insertScore(int playerId, int score, int totalLines, int level, int durationSeconds);
    std::vector<std::pair<std::string, int>> fetchTopScores(int topN);

    bool insertGameStats(
        int playerId,
        int score,
        int durationSeconds,
        int level,
        const int pieceCounts[7],
        int totalLines
    );
};