#include <iostream>

#include <wing/WingMySQL.h>

static auto on_complete(wing::Query request) -> void
{
    std::cout << "Finished query: " << request->GetQueryWithBindParams() << std::endl;
    std::cout << wing::query_status2str(request->GetQueryStatus()) << "\n";
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

int main(int argc, char* argv[])
{
    if(argc < 8)
    {
        std::cout << argv[0] << " <hostname> <port> <user> <password> <db> <query> <...params>\n";
        return 1;
    }

    std::string hostname(argv[1]);
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));
    std::string user(argv[3]);
    std::string password(argv[4]);
    std::string db(argv[5]);
    std::string mysql_query_string(argv[6]);

    wing::startup();

    wing::ConnectionInfo connection(hostname, port, user, password, db, 0);
    wing::EventLoop event_loop(connection);

    using namespace std::chrono_literals;
    auto& query_pool = event_loop.GetQueryPool();
    auto query = query_pool.Produce(mysql_query_string, 1000ms, on_complete);

    for(size_t i = 7; i < static_cast<size_t>(argc); ++i)
    {
        // we'll only support strings for the example
        query->BindString(argv[i]);
    }

    event_loop.StartQuery(std::move(query));

    std::this_thread::sleep_for(1000ms);
    while(event_loop.GetActiveQueryCount() > 0)
    {
        std::this_thread::sleep_for(100ms);
    }

    event_loop.Stop();

    wing::shutdown();

    return 0;
}
