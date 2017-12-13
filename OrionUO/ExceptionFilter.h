//----------------------------------------------------------------------------------
#ifndef EXCEPTIONFILTER_H
#define EXCEPTIONFILTER_H
//----------------------------------------------------------------------------------
void DumpCurrentRegistersInformation(CONTEXT* CR);
void DumpLibraryInformation();
LONG __stdcall OrionUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo);
//----------------------------------------------------------------------------------
#endif //EXCEPTIONFILTER_H
//----------------------------------------------------------------------------------
