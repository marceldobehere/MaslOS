#include "MStackS.h"
#include "../../OSDATA/osdata.h"

void PrintMStackTrace(MStack stack[], int64_t size, BasicRenderer* renderer, uint32_t col);
void PrintMStackTrace(MStack stack[], int64_t size);

void AddToMStack(MStack thing);
void RemoveLastMStack();