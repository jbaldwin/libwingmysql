#include <iostream>

#include <wing/WingMySQL.h>

static auto on_complete(wing::QueryHandle request) -> void
{
    std::cout << "Finished query: " << request->GetQueryOriginal() << std::endl;
    std::cout << wing::to_string(request->GetQueryStatus()) << "\n";
    if(request->HasError())
    {
        std::cout << "Error: " << request->GetError() << "\n";
    }
    else
    {
        std::cout << "Fields count: " << request->GetFieldCount() << "\n";
        std::cout << "Row count: " << request->GetRowCount() << "\n";
        for(size_t row_idx = 0; row_idx < request->GetRowCount(); ++row_idx)
        {
            auto& row = request->GetRow(row_idx);
            for(size_t col_idx = 0; col_idx < row.GetColumnCount(); ++col_idx)
            {
                auto& value = row.GetColumn(col_idx);
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
    wing::Statement mysql_statement;
    mysql_statement << argv[6];

    wing::startup();

    wing::ConnectionInfo connection(hostname, port, user, password, db, 0);
    wing::EventLoop event_loop(connection);

    using namespace std::chrono_literals;
    auto& request_pool = event_loop.GetQueryPool();
    auto request = request_pool.Produce(mysql_statement, 1000ms, on_complete);
    event_loop.StartQuery(std::move(request));

    std::this_thread::sleep_for(250ms);
    while(event_loop.GetActiveQueryCount() > 0)
    {
        std::this_thread::sleep_for(100ms);
    }

    request = request_pool.Produce(mysql_statement, 1000ms, on_complete);
    event_loop.StartQuery(std::move(request));
    std::this_thread::sleep_for(250ms);
    while(event_loop.GetActiveQueryCount() > 0)
    {
        std::this_thread::sleep_for(100ms);
    }

    event_loop.Stop();

    wing::shutdown();

    return 0;
}
