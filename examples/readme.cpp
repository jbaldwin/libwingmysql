#include <iostream>

#include <wing/WingMySQL.hpp>

int main(int argc, char* argv[])
{
    using namespace std::chrono_literals;
    if (argc < 7) {
        std::cout << argv[0] << " <hostname> <port> <user> <password> <db> <query>\n";
        return 1;
    }

    // Create connection information to the mysql database.
    wing::ConnectionInfo connection {
        argv[1],
        static_cast<uint16_t>(std::stoi(argv[2])),
        argv[3],
        argv[4],
        argv[5],
        0 };

    // Create a statement, normally this would have wing::Statement::Arg()
    // around parameters that should be bound, this is a simple example.
    wing::Statement statement {};
    statement << argv[6];

    // Create an executor with the connection information and then start
    // a query from the statement.  This query is given a timeout of 30s.
    // StartQuery() can fail, and thus returns std::optional, if this holds
    // the std::future<wing::QueryHandle> then the query is executing, wait
    // on the future to complete for the query results.
    wing::Executor executor { std::move(connection) };
    auto query_handle = executor.StartQuery(std::move(statement), 30s).value().get();

    // If the query succeeded print the row contents, all columns can be viewed
    // via AsStringView() but there are As<Type> functions to have wing convert
    // the column value into the expected type for you.
    if (query_handle->QueryStatus() == wing::QueryStatus::SUCCESS) {
        std::cout << "Fields count: " << query_handle->FieldCount() << "\n";
        std::cout << "Row count: " << query_handle->RowCount() << "\n";

        for (const auto& row : query_handle->Rows()) {
            for (const auto& value : row.Columns()) {
                if (value.IsNull()) {
                    std::cout << "NULL ";
                } else {
                    std::cout << value.AsStringView() << " ";
                }
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "An error occurred: " << wing::to_string(query_handle->QueryStatus()) << "\n";
    }

    std::string to_be_escaped = "ESCAPE_ME";
    int64_t i_wont_be_escaped = 5;
    using Arg = wing::Statement::Arg;
    wing::Statement statement2 {};
    statement2 << "SELECT column FROM DATABASE WHERE name like " << Arg(to_be_escaped) << " AND id = " << Arg(i_wont_be_escaped);

    return 0;
}
