#pragma once
#include <stdint.h>

bool StrEquals(const char* a, const char* b);

char* StrCopy(const char* og);

char* StrSubstr(const char* og, int index, int len);

char* StrSubstr(const char* og, int index);

int32_t StrLen(const char* og);