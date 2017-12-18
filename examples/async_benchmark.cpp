#include <iostream>
#include <atomic>

#include <wing/WingMySQL.h>

static std::atomic<uint64_t> count{0};
static std::atomic<uint64_t> errors{0};

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
    }

    // if the request status was good continue
    if(request->GetQueryStatus() == wing::QueryStatus::SUCCESS)
    {
        auto* event_loop = static_cast<wing::EventLoop*>(request->GetUserData());
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

    auto duration = std::chrono::seconds(std::stoul(argv[1]));
    size_t connections = std::stoul(argv[2]);
    std::string hostname(argv[3]);
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[4]));
    std::string user(argv[5]);
    std::string password(argv[6]);
    std::string db(argv[7]);
    std::string mysql_query_string(argv[8]);

    wing::startup();

    wing::ConnectionInfo connection(hostname, port, user, password, db, 0);
    wing::EventLoop event_loop(connection);
    auto& request_pool = event_loop.GetQueryPool();

    using namespace std::chrono_literals;
    for(size_t i = 0; i < connections; ++i) {
        auto request = request_pool.Produce(mysql_query_string, 1000ms, on_complete);
        request->SetUserData(&event_loop);
        event_loop.StartQuery(std::move(request));
    }

    std::this_thread::sleep_for(duration);

    event_loop.Stop();

    wing::shutdown();

    print_stats(static_cast<uint64_t>(duration.count()), 1, count, errors);

    return 0;
}
