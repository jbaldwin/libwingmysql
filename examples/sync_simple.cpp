#include <iostream>

#include <wing/WingMySQL.h>

static auto run_query(wing::QueryHandle& query) -> void
{
    auto status = query->Execute();
    if(status == wing::QueryStatus::SUCCESS)
    {
        std::cout << "Fields count: " << query->GetFieldCount() << "\n";
        std::cout << "Row count: " << query->GetRowCount() << "\n";
        for(size_t row_idx = 0; row_idx < query->GetRowCount(); ++row_idx)
        {
            auto& row = query->GetRow(row_idx);
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
    else
    {
        std::cout << "An error occurred: " << wing::to_string(status) << "\n";
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

    using namespace std::chrono_literals;

    wing::QueryPool query_pool(connection);
    {
        auto query = query_pool.Produce(mysql_statement, 1000ms);
        run_query(query);
        run_query(query);
    }
    {
        auto query = query_pool.Produce(mysql_statement, 1000ms);
        run_query(query);
    }

    wing::shutdown();

    return 0;
}
