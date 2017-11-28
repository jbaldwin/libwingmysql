#include <iostream>

#include <wing/WingMySQL.h>

auto run_query(wing::Query& query) -> void
{
    auto status = query->Execute();
    if(status == wing::QueryStatus::SUCCESS)
    {
        std::cout << "Success : fields[" << query->GetFieldCount() << "] rows[" << query->GetRowCount() << "]\n";
        for(auto& row : query->GetRows())
        {
            for(auto& value : row.GetValues())
            {
                std::cout << value.AsString() << " ";
            }
            std::cout << "\n";
        }
    }
    else
    {
        std::cout << "An error occurred: " << wing::query_status2str(status) << "\n";
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
    std::string mysql_query_string(argv[6]);

    wing::startup();

    wing::ConnectionInfo connection(hostname, port, user, password, db, 0);

    using namespace std::chrono_literals;

    wing::QueryPool query_pool(connection);
    {
        auto query = query_pool.Produce(mysql_query_string, 1000ms);
        run_query(query);
        run_query(query);
    }
    {
        auto query = query_pool.Produce(mysql_query_string, 1000ms);
        run_query(query);
    }


    wing::shutdown();

    return 0;
}
