#pragma once
#include <stdint.h>

bool StrEquals(const char* a, const char* b);

char* StrCopy(const char* og);

char* StrSubstr(const char* og, int index, int len);

char* StrSubstr(const char* og, int index);

bool StrStartsWith(const char* og, const char* with);
bool StrEndsWith(const char* og, const char* with);

char* StrCombine(const char* a, const char* b);
char* StrCombine(const char* a, const char* b, const char* c);
char* StrCombine(const char* a, const char* b, const char* c, const char* d);

char* StrReplaceStartingStuffWith(const char* og, const char* toReplace, const char* toReplaceWith);

uint64_t StrCountChr(const char* og, char chr);

int32_t StrLen(const char* og);

int32_t StrIndexOf(const char* str, char chr);
int32_t StrLastIndexOf(const char* str, char chr);

int32_t StrIndexOf(const char* str, char chr, int ignoreCount);
int32_t StrLastIndexOf(const char* str, char chr, int ignoreCount);
