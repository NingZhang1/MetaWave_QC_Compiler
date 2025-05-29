/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 08:56:53
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 22:27:27
 */

#include <vector>
#include <string>
#include <tuple>
#include <sstream>
#include <ostream>

//----------------------------------
// error
//----------------------------------

namespace MetaWaveCompiler
{

    class MetaWaveException : public std::exception
    {
    public:
        explicit MetaWaveException(std::string message);
        const char *what() const noexcept;

    private:
        std::string message;
    };

    /// Error report (based on Halide's Error.h)
    struct ErrorReport
    {
        enum Kind
        {
            User,
            Internal,
            Temporary
        };

        std::ostringstream *msg;
        const char *file;
        const char *func;
        int line;

        bool condition;
        const char *conditionString;

        Kind kind;
        bool warning;

        ErrorReport(const char *file, const char *func, int line, bool condition, const char *conditionString, Kind kind, bool warning);

        template <typename T>
        ErrorReport &operator<<(T x)
        {
            if (condition)
            {
                return *this;
            }
            (*msg) << x;
            return *this;
        }

        ErrorReport &operator<<(std::ostream &(*manip)(std::ostream &))
        {
            if (condition)
            {
                return *this;
            }
            (*msg) << manip;
            return *this;
        }

        ~ErrorReport() noexcept(false)
        {
            if (condition)
            {
                return;
            }
            explodeWithException();
        }

        void explodeWithException();
    };

// internal asserts
#ifdef METAWAVE_ASSERTS
#define metawave_iassert(c) MetaWaveCompiler::ErrorReport(__FILE__, __FUNCTION__, __LINE__, (c), #c, MetaWaveCompiler::ErrorReport::Internal, false)
#define metawave_ierror MetaWaveCompiler::ErrorReport(__FILE__, __FUNCTION__, __LINE__, false, NULL, MetaWaveCompiler::ErrorReport::Internal, false)
#else
    struct Dummy
    {
        template <typename T>
        Dummy &operator<<(T x)
        {
            return *this;
        }
        // Support for manipulators, such as std::endl
        Dummy &operator<<(std::ostream &(*manip)(std::ostream &)) { return *this; }
    };

#define metawave_iassert(c) MetaWaveCompiler::Dummy()
#define metawave_ierror MetaWaveCompiler::Dummy()
#endif

#define metawave_unreachable metawave_ierror << "reached unreachable location"

// User asserts
#define metawave_uassert(c) MetaWaveCompiler::ErrorReport(__FILE__, __FUNCTION__, __LINE__, (c), #c, MetaWaveCompiler::ErrorReport::User, false)
#define metawave_uerror MetaWaveCompiler::ErrorReport(__FILE__, __FUNCTION__, __LINE__, false, nullptr, MetaWaveCompiler::ErrorReport::User, false)
#define metawave_uwarning MetaWaveCompiler::ErrorReport(__FILE__, __FUNCTION__, __LINE__, false, nullptr, MetaWaveCompiler::ErrorReport::User, true)

// Temporary assertions (planned for the future)
#define metawave_tassert(c) MetaWaveCompiler::ErrorReport(__FILE__, __FUNCTION__, __LINE__, (c), #c, MetaWaveCompiler::ErrorReport::Temporary, false)
#define metawave_terror MetaWaveCompiler::ErrorReport(__FILE__, __FUNCTION__, __LINE__, false, NULL, MetaWaveCompiler::ErrorReport::Temporary, false)

#define metawave_not_supported_yet metawave_uerror << "Not supported yet"

} // namespace MetaWaveCompiler

//----------------------------------
// error message
//----------------------------------

#ifdef XXXXXXXXX

namespace MetaWaveCompiler
{
    namespace error
    {

        // unsupported type bit width error
        extern const std::string type_mismatch;
        extern const std::string type_bitwidt;

        // TensorVar::setIndexExpression error messages
        extern const std::string expr_dimension_mismatch;
        extern const std::string expr_transposition;
        extern const std::string expr_distribution;
        extern const std::string expr_einsum_missformed;

        // compile error messages
        extern const std::string compile_without_expr;
        extern const std::string compile_tensor_name_collision;

        // assemble error messages
        extern const std::string assemble_without_compile;

        // compute error messages
        extern const std::string compute_without_compile;

        // factory function error messages
        extern const std::string requires_matrix;

#define INIT_REASON(reason)    \
    string reason_;            \
    do                         \
    {                          \
        if (reason == nullptr) \
        {                      \
            reason = &reason_; \
        }                      \
        *reason = "";          \
    } while (0)

    } // namespace error
} // namespace MetaWaveCompiler

//----------------------------------
// error checks
//----------------------------------

namespace MetaWaveCompiler
{
    class IndexVar;
    class IndexExpr;
    class Format;
    class Shape;

    namespace error
    {

        /// Check whether all dimensions indexed by the same variable are the same.
        /// If they are not, then the first element of the returned tuple will be false,
        /// and a human readable error will be returned in the second component.
        std::pair<bool, std::string> dimensionsTypecheck(const std::vector<IndexVar> &resultVars, const IndexExpr &expr, const Shape &shape);

        /// Returns true iff the index expression contains a transposition.
        bool containsTranspose(const Format &resultFormat, const std::vector<IndexVar> &resultVars, const IndexExpr &expr);

    } // namespace error
} // namespace MetaWaveCompiler

#endif