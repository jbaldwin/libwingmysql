#include <iostream>

#include <wing/WingMySQL.hpp>

static auto on_complete(wing::QueryHandle request) -> void
{
    std::cout << "Finished query: " << request->QueryOriginal() << std::endl;
    std::cout << wing::to_string(request->QueryStatus()) << "\n";
    if (request->Error().has_value()) {
        std::cout << "Error: " << request->Error().value() << "\n";
    } else {
        std::cout << "Fields count: " << request->FieldCount() << "\n";
        std::cout << "Row count: " << request->RowCount() << "\n";
        for (size_t row_idx = 0; row_idx < request->RowCount(); ++row_idx) {
            auto& row = request->Row(row_idx);
            for (size_t col_idx = 0; col_idx < row.ColumnCount(); ++col_idx) {
                auto& value = row.Column(col_idx);
                if (value.IsNull()) {
                    std::cout << "NULL ";
                } else {
                    std::cout << value.AsStringView().value() << " ";
                }
            }
            std::cout << "\n";
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc < 7) {
        std::cout << argv[0] << " <hostname> <port> <user> <password> <db> <query>\n";
        return 1;
    }

    std::string hostname { argv[1] };
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));
    std::string user { argv[3] };
    std::string password { argv[4] };
    std::string db { argv[5] };
    wing::Statement statement;
    statement << argv[6];

    wing::ConnectionInfo connection { hostname, port, user, password, db, 0 };

    // Execute two queries asynchronously on the same executor.
    wing::Executor executor { std::move(connection), 2 };

    using namespace std::chrono_literals;
    (void)executor.StartQuery(statement, 1000ms, on_complete);
    (void)executor.StartQuery(statement, 1000ms, on_complete);

    return 0;
}
