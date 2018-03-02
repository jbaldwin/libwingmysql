#include <iostream>
#include <atomic>

#include <wing/WingMySQL.h>

static std::atomic<uint64_t> count{0};
static std::atomic<uint64_t> errors{0};

static std::chrono::seconds duration;
static size_t connections;
static std::string hostname;
static uint16_t port;
static std::string user;
static std::string password;
static std::string db;
static wing::Statement mysql_statement;

using namespace std::chrono_literals;

static auto print_stats(
    uint64_t duration_s,
    uint64_t threads,
    uint64_t total_success,
    uint64_t total_error
) -> void
{
    auto total = total_success + total_error;
    std::cout << "  Thread Stats    Avg\n";
    std::cout << "    Req/sec     " << (total / static_cast<double>(threads) / duration_s) << "\n";

    std::cout << "  " << total << " requests in " << duration_s << "s\n";
    if(total_error > 0)
    {
        std::cout << "  " << total_error << " errors\n";
    }
    std::cout << "Requests/sec: " << (total / static_cast<double>(duration_s)) << "\n";
}

static auto on_complete(wing::Query request) -> void
{
    ++count;
    if(request->HasError())
    {
        ++errors;
        std::cout << "ERROR: " << wing::to_string(request->GetQueryStatus()) << " " << request->GetError() << "\n";
    }

    auto* event_loop = static_cast<wing::EventLoop*>(request->GetUserData());
    if(request->GetQueryStatus() == wing::QueryStatus::SUCCESS)
    {
        event_loop->StartQuery(std::move(request));
    }
    else
    {
        auto& request_pool = event_loop->GetQueryPool();
        request = request_pool.Produce(mysql_statement, 1000ms, on_complete);
        request->SetUserData(event_loop);
        event_loop->StartQuery(std::move(request));

    }
}

int main(int argc, char* argv[])
{
    if(argc < 9)
    {
        std::cout << argv[0] << " <duration_seconds> <connections> <hostname> <port> <user> <password> <db> <query>\n";
        return 1;
    }

    duration = std::chrono::seconds(std::stoul(argv[1]));
    connections = std::stoul(argv[2]);
    hostname = (argv[3]);
    port = static_cast<uint16_t>(std::stoi(argv[4]));
    user = (argv[5]);
    password = (argv[6]);
    db = (argv[7]);
    mysql_statement << argv[8];

    wing::startup();

    wing::ConnectionInfo connection(hostname, port, user, password, db, 0);
    wing::EventLoop event_loop(connection);
    auto& request_pool = event_loop.GetQueryPool();

    for(size_t i = 0; i < connections; ++i) {
        auto request = request_pool.Produce(mysql_statement, 1000ms, on_complete);
        request->SetUserData(&event_loop);
        event_loop.StartQuery(std::move(request));
    }

    std::this_thread::sleep_for(duration);

    event_loop.Stop();

    wing::shutdown();

    print_stats(static_cast<uint64_t>(duration.count()), 1, count, errors);

    return 0;
}
