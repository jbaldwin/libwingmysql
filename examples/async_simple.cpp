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
                    std::cout << value.AsStringView() << " ";
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

    std::string hostname(argv[1]);
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));
    std::string user(argv[3]);
    std::string password(argv[4]);
    std::string db(argv[5]);
    wing::Statement mysql_statement;
    mysql_statement << argv[6];

    wing::GlobalScopeInitializer wing_gsi{};

    wing::ConnectionInfo connection(hostname, port, user, password, db, 0);
    wing::EventLoop event_loop(connection);

    using namespace std::chrono_literals;
    auto request = event_loop.ProduceQuery(mysql_statement, 1000ms, on_complete);
    event_loop.StartQuery(std::move(request));

    std::this_thread::sleep_for(250ms);
    while (event_loop.ActiveQueryCount() > 0) {
        std::this_thread::sleep_for(100ms);
    }

    request = event_loop.ProduceQuery(mysql_statement, 1000ms, on_complete);
    event_loop.StartQuery(std::move(request));
    std::this_thread::sleep_for(250ms);
    while (event_loop.ActiveQueryCount() > 0) {
        std::this_thread::sleep_for(100ms);
    }

    event_loop.Stop();

    return 0;
}
