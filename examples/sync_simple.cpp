#include <iostream>

#include <wing/WingMySQL.hpp>

static auto run_query(wing::QueryHandle& query) -> void
{
    auto status = query->Execute();
    if (status == wing::QueryStatus::SUCCESS) {
        std::cout << "Fields count: " << query->FieldCount() << "\n";
        std::cout << "Row count: " << query->RowCount() << "\n";
        for (size_t row_idx = 0; row_idx < query->RowCount(); ++row_idx) {
            auto& row = query->Row(row_idx);
            for (size_t col_idx = 0; col_idx < row.ColumnCount(); ++col_idx) {
                auto& value = row.Column(col_idx);
                if (value.IsNull()) {
                    std::cout << "NULL ";
                } else {
                    std::cout << value.AsStringView() << " ";
                }
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "An error occurred: " << wing::to_string(status) << "\n";
    }
}

int main(int argc, char* argv[])
{
    if (argc < 7) {
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

    wing::GlobalScopeInitializer wing_gsi{};

    wing::ConnectionInfo connection{ hostname, port, user, password, db, 0 };

    using namespace std::chrono_literals;

    wing::QueryPool query_pool{ connection };
    {
        auto query = query_pool.Produce(mysql_statement, 1000ms);
        run_query(query);
        run_query(query);
    }
    {
        auto query = query_pool.Produce(mysql_statement, 1000ms);
        run_query(query);
    }

    return 0;
}
