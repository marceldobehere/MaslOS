#pragma once
#include <stdint.h>

bool StrEquals(const char* a, const char* b);

char* StrCopy(const char* og);

char* StrSubstr(const char* og, int index, int len);

char* StrSubstr(const char* og, int index);

bool StrStartsWith(const char* og, const char* with);

char* StrCombine(const char* a, const char* b);

uint64_t StrCountChr(const char* og, char chr);

int32_t StrLen(const char* og);