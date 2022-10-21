#pragma once
#include <stdint.h>

enum varType
{
    _int, 
    _int64, 
    _uint64, 
    _double,
    _bool,
    _charPointer,
};

struct dispVar
{
    varType type;
    int _int;
    int64_t _int64;
    uint64_t _uint64;
    double _double;
    bool _bool;
    const char* _charPointer;

    dispVar(int _int)
    {
        this->_int = _int;
        type = varType::_int;
    }

    dispVar(int64_t _int64)
    {
        this->_int64 = _int64;
        type = varType::_int64;
    }

    dispVar(uint64_t _uint64)
    {
        this->_uint64 = _uint64;
        type = varType::_uint64;
    }

    dispVar(double _double)
    {
        this->_double = _double;
        type = varType::_double;
    }

    dispVar(bool _bool)
    {
        this->_bool = _bool;
        type = varType::_bool;
    }

    dispVar(const char* _charPointer)
    {
        this->_charPointer = _charPointer;
        type = varType::_charPointer;
    }
};

const char* PrintArg(dispVar var);