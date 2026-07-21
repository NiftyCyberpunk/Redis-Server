# pragma once

# include <atomic>
# include <chrono>
# include <cstdint>

struct ServerStats {
    std::chrono::steady_clock::time_point startTime;
    std::atomic<uint64_t> totalCommands = 0;
    std::atomic<uint64_t> connectedClients = 0;
};