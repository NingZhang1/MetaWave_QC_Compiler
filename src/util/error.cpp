/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 08:58:11
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 22:34:46
 */

#include "../include/util/error.h"

#include <iostream>
#include <cstdlib>

using namespace std;

//----------------------------------
// error
//----------------------------------

namespace MetaWaveCompiler
{

    MetaWaveException::MetaWaveException(std::string msg) : message(msg) {}

    const char *MetaWaveException::what() const noexcept { return message.c_str(); }

    ErrorReport::ErrorReport(const char *file, const char *func, int line, bool condition, const char *conditionString, Kind kind, bool warning)
        : msg(NULL), file(file), func(func), line(line), condition(condition), conditionString(conditionString), kind(kind), warning(warning)
    {
        if (condition)
        {
            return;
        }
        msg = new std::ostringstream;

        switch (kind)
        {
        case User:
            if (warning)
            {
                (*msg) << "Warning";
            }
            else
            {
                (*msg) << "Error";
            }
            (*msg) << " at " << file << ":" << line << " in " << func << ":" << endl;
            break;
        case Internal:
            (*msg) << "Compiler bug";
            if (warning)
            {
                (*msg) << "(warning)";
            }
            (*msg) << " at " << file << ":" << line << " in " << func;
            (*msg) << endl
                   << "Please report it to developers";

            if (conditionString)
            {
                (*msg) << endl
                       << " Condition failed: " << conditionString;
            }
            (*msg) << endl;
            break;
        case Temporary:
            (*msg) << "Temporary assumption broken";
            (*msg) << " at " << file << ":" << line << endl;
            (*msg) << " Not supported yet, but planned for the future";
            if (conditionString)
            {
                (*msg) << endl
                       << " Condition failed: " << conditionString;
            }
            (*msg) << endl;
            break;
        }
        (*msg) << " ";
    }

    void ErrorReport::explodeWithException()
    {
        MetaWaveException e = MetaWaveException(msg->str());
        delete msg;
        throw e;
    }

} // namespace MetaWaveCompiler

#ifdef XXXXXXXXX

//----------------------------------
// error message
//----------------------------------

namespace MetaWaveCompiler
{
    namespace error
    {

        const std::string type_mismatch = "Type mismatch.";

        const std::string type_bitwidt = "The given bit width is not supported for this type.";

        const std::string expr_dimension_mismatch = "Dimension size mismatch.";

        const std::string expr_transposition = "Computations with transpositions are not supported, but are planned for the "
                                               "future.";

        const std::string expr_distribution = "Expressions with free variables that do not appear on the right hand side "
                                              "of the expression are not supported, but are planned for the future";

        const std::string expr_einsum_missformed = "Summations/reductions are not specified and the Einstein summation "
                                                   "convention cannot be applied, because the expression is not of the form "
                                                   "a*...*b + ... + c*...*d.";

        const std::string compile_without_expr = "The tensor must be assigned to before compile is called.";

        const std::string compile_tensor_name_collision = "Tensor name collision.";

        const std::string assemble_without_compile = "The compile method must be called before assemble.";

        const std::string compute_without_compile = "The compile method must be called before compute.";

        const std::string requires_matrix = "The argument must be a matrix.";

    } // namespace error
} // namespace MetaWaveCompiler

#endif