#include <iostream>

#include <wing/WingMySQL.h>

class RequestCallback : public wing::IQueryCallback
{
public:
    auto OnComplete(wing::Query request) -> void
    {
        std::cout << "Finished query: " << request->GetQueryOriginal() << std::endl;
        std::cout << wing::query_status2str(request->GetRequestStatus()) << "\n";
        if(request->HasError())
        {
            std::cout << "Error: " << request->GetError() << "\n";
        }
        else
        {
            std::cout << "Fields count: " << request->GetFieldCount() << "\n";
            std::cout << "Row count: " << request->GetRowCount() << "\n";
            for(const auto& row : request->GetRows())
            {
                for(const auto& value : row.GetValues())
                {
                    if(value.IsNull())
                    {
                        std::cout << "NULL ";
                    }
                    else
                    {
                        std::cout << value.AsString() << " ";
                    }
                }
                std::cout << "\n";
            }
        }
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

    wing::Connection connection(hostname, port, user, password, db, 0);

    wing::EventLoop event_loop(
        std::make_unique<RequestCallback>(),
        connection
    );

    using namespace std::chrono_literals;
    auto& request_pool = event_loop.GetQueryPool();
    auto request = request_pool.Produce(query, 1000ms);

    event_loop.StartQuery(std::move(request));

    std::this_thread::sleep_for(1000ms);
    while(event_loop.GetActiveQueryCount() > 0)
    {
        std::this_thread::sleep_for(100ms);
    }

    event_loop.Stop();

    wing::shutdown();

    return 0;
}