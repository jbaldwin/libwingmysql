#include <iostream>

#include <wing/WingMySQL.h>

class RequestCallback : public wing::IRequestCallback
{
public:
    auto OnComplete(wing::Request request) -> void
    {
        std::cout << "Finished query: " << request->GetQuery() << std::endl;
        std::cout << wing::request_status2str(request->GetRequestStatus()) << "\n";
    }
};

int main(int argc, char* argv[])
{
    if(argc < 7)
    {
        std::cout << argv[0] << " <hostname> <port> <user> <password> <db> <query>\n";
        return 1;
    }

    std::string hostname(argv[1]);
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));
    std::string user(argv[3]);
    std::string password(argv[4]);
    std::string db(argv[5]);
    std::string query(argv[6]);

    wing::startup();

    wing::EventLoop event_loop(
        std::make_unique<RequestCallback>(),
        hostname,
        port,
        user,
        password,
        db,
        0
    );

    using namespace std::chrono_literals;
    auto& request_pool = event_loop.GetRequestPool();
    auto request = request_pool.Produce(
        query,
        1000ms
    );

    event_loop.StartRequest(std::move(request));

    std::this_thread::sleep_for(1000ms);
    while(event_loop.GetActiveRequestCount() > 0)
    {
        std::this_thread::sleep_for(100ms);
    }

    event_loop.Stop();

    wing::shutdown();

    return 0;
}