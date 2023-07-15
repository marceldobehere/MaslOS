#include "dispTypes.h"
#include "../../cStdLib/cstr.h"

const char* PrintArg(dispVar var)
{
    const char* toPrint = "<IDK>";
    if (var.type == varType::_bool)
        toPrint = to_string(var._bool);
    else if (var.type == varType::_int)
        toPrint = to_string(var._int);
    else if (var.type == varType::_int64)
        toPrint = to_string(var._int64);
    else if (var.type == varType::_uint64)
        toPrint = to_string(var._uint64);
    else if (var.type == varType::_double)
        toPrint = to_string(var._double);
    else if (var.type == varType::_charPointer)
        toPrint = var._charPointer;

    return toPrint;
}