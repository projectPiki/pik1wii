#ifndef _MSL_WSTRING_H
#define _MSL_WSTRING_H

#include "types.h"

BEGIN_SCOPE_EXTERN_C

size_t wcslen(const wchar_t*);
wchar_t* wcscpy(wchar_t*, const wchar_t*);
wchar_t* wcsncpy(wchar_t*, const wchar_t*, size_t);
wchar_t* wcscat(wchar_t*, const wchar_t*);
int wcscmp(const wchar_t*, const wchar_t*);
wchar_t* wcschr(const wchar_t*, wchar_t);

END_SCOPE_EXTERN_C

#endif
