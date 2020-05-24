#include <atomic>
#include <iostream>

#include <wing/WingMySQL.hpp>

static std::atomic<uint64_t> count { 0 };
static std::atomic<uint64_t> errors { 0 };

static std::chrono::seconds duration;
static size_t threads;
static size_t connections;
static std::string hostname;
static uint16_t port;
static std::string user;
static std::string password;
static std::string db;
static wing::Statement statement;

using namespace std::chrono_literals;
using namespace std::placeholders;

static auto print_stats(
    uint64_t duration_s,
    uint64_t threads,
    uint64_t total_success,
    uint64_t total_error) -> void
{
    auto total = total_success + total_error;
    std::cout << "  Thread Stats    Avg\n";
    std::cout << "    Req/sec     " << (total / static_cast<double>(threads) / duration_s) << "\n";

    std::cout << "  " << total << " requests in " << duration_s << "s\n";
    if (total_error > 0) {
        std::cout << "  " << total_error << " errors\n";
    }
    std::cout << "Requests/sec: " << (total / static_cast<double>(duration_s)) << "\n";
}

static auto on_complete(wing::QueryHandle request, wing::Executor& executor) -> void
{
    ++count;
    if (request->Error().has_value()) {
        ++errors;
        std::cout << "ERROR: " << wing::to_string(request->QueryStatus()) << " " << request->Error().value() << "\n";
    }

    if (request->QueryStatus() == wing::QueryStatus::SUCCESS) {
        (void)executor.StartQuery(
            statement,
            1000ms,
            [&executor](wing::QueryHandle qh) {
                on_complete(std::move(qh), executor);
            });
    } else {
        /// libwingmysql doesn't let you restart a failed request
        (void)executor.StartQuery(statement, 1000ms, [&executor](wing::QueryHandle qh) {
            on_complete(std::move(qh), executor);
        });
    }
}

int main(int argc, char* argv[])
{
    if (argc < 10) {
        std::cout << argv[0] << " <duration_seconds> <threads> <connections> <hostname> <port> <user> <password> <db> <query>\n";
        return 1;
    }

    duration = std::chrono::seconds(std::stoul(argv[1]));
    threads = std::stoul(argv[2]);
    connections = std::stoul(argv[3]);
    hostname = argv[4];
    port = static_cast<uint16_t>(std::stoi(argv[5]));
    user = argv[6];
    password = argv[7];
    db = argv[8];
    statement << argv[9];

    wing::ConnectionInfo connection { hostname, port, user, password, db, 0 };
    wing::Executor executor { std::move(connection), threads };

    for (size_t i = 0; i < connections; ++i) {
        (void)executor.StartQuery(statement, 1000ms, [&executor](wing::QueryHandle qh) {
            on_complete(std::move(qh), executor);
        });
    }

    std::this_thread::sleep_for(duration);

    executor.Stop();

    print_stats(static_cast<uint64_t>(duration.count()), threads, count, errors);

    return 0;
}
