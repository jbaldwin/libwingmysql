#pragma once

#include <string>
#include <vector>

namespace wing
{

/**
 * This object encapsulates a statement to be used for a query,
 * and can be created from a raw string or be used to create
 * a prepared statement by binding parameters
 */
class Statement
{
    friend class QueryHandle;
public:
    /**
     * Arg structure, which can be constructed and streamed into a Statement to provide
     * automatic deduction for whether an argument needs to be escaped
     */
    struct Arg {

        /**
         * Creates a bound argument of an arbitary string convertible (via streams) type
         *
         * Note that strings do not automatically include quotes when binding, you must
         * include them in the template query.
         * @tparam T the type of the value to bind
         * @param parameter_value the value to bind
         */
        template <typename T>
        Arg(const T& parameter_value);

        std::string m_string_value;
        bool m_requires_escaping;
    };

    ~Statement() = default;
    Statement() = default;

    Statement(const Statement&) = default;                      ///< Can copy
    Statement(Statement&&) = default;                           ///< Can move
    auto operator = (const Statement&) -> Statement& = default; ///< Can copy assign
    auto operator = (Statement&&) -> Statement& = default;      ///< Can move assign

    /**
     * Stream operator to accept raw query parts - these pieces are never escaped!
     *
     * Use this for streaming in parts of the statement that are static and don't
     * originate from outside sources (e.g. query/post params, database, sockets, etc)
     * @tparam T arbitrary streamable type
     * @param statement_part the data to append to the statement
     * @return a reference to this Statement
     */
    template <typename T>
    auto operator << (const T& statement_part) -> Statement&;

    /**
     * Stream operator to accept arguments - these will be automatically escaped
     * as required.
     *
     * Use this for streaming in parts of the statement that are dynamic or may
     * originate from outside sources
     * @param parameter the argument ot append to the statement
     * @return a reference to this statement
     */
    auto operator << (Arg parameter) -> Statement&;
private:
    /**
     * Internal struct, similar to Arg but does not perform conversion and does
     * not deduce if escaping is required.
     *
     * Both stream operators will create StatementPart objects and emplace them
     * into the m_statement_parts vector
     */
    struct StatementPart {
        StatementPart(std::string value, bool requires_escaping);

        std::string m_string_value;
        bool m_requires_escaping;
    };

    std::vector<StatementPart> m_statement_parts; ///< all parts of the statement, bound or otherwise

    /**
     * Prepares a final string statement for use in MySQL by escaping all bound
     * parameters that require escaping using the providing escaping functor
     * @tparam EscapeFunctor Functor type that can receive a string and returns an
     *                       escaped version of that string for query building
     * @param escape_functor the functor to call to escape values
     * @return the final prepared statement
     */
    template <typename EscapeFunctor>
    auto prepareStatement(EscapeFunctor escape_functor) -> std::string;
};

} // wing

#include "wing/Statement.tcc"
