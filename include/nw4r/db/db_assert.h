#ifndef NW4R_DB_ASSERT_H
#define NW4R_DB_ASSERT_H

#include <nw4r/types_nw4r.h>
#include <stdarg.h>

namespace nw4r {
namespace db {

// Forward declarations
namespace detail {
    class ConsoleHead;
}

// TODO: do these macros belong here?

#ifdef NW4R_LOG
#define NW4R_WARNING(line, ...) nw4r::db::Warning(__FILE__, line, __VA_ARGS__)
#define NW4R_PANIC(line, ...) nw4r::db::Panic(__FILE__, line, __VA_ARGS__)
#define NW4R_DB_ASSERT(exp, line, ...) \
    if(!(exp))                \
    nw4r::db::Panic(__FILE__, line, __VA_ARGS__)

#define NW4R_ASSERT_MSG(x, line, ...)                                                \
    ((x) && 1 || (nw4r::db::Panic(__FILE__, line, __VA_ARGS__), 0))
#else
#define NW4R_WARNING(line, ...) (void)(0)
#define NW4R_PANIC(line, ...) (void)(0)
#define NW4R_DB_ASSERT(exp, line, ...) (void)(0)
#define NW4R_ASSERT_MSG(x, line, ...) (void)(0)
#endif

#define NW4R_ASSERT(x, line)                                                    \
    NW4R_ASSERT_MSG((x), line, "NW4R:Failed assertion " #x)

#define NW4R_ASSERT_PTR_NULL(ptr, line)                                                     \
    NW4R_ASSERT_MSG((ptr) != 0, \
                        line,                                                  \
                    "NW4R:Pointer must not be NULL (" #ptr ")")

#define NW4R_ASSERT_PTR(x, line)                                                     \
    NW4R_ASSERT_MSG((((u32)(x) & 0xff000000) == 0x80000000) ||                 \
                        (((u32)(x) & 0xff800000) == 0x81000000) ||             \
                        (((u32)(x) & 0xf8000000) == 0x90000000) ||             \
                        (((u32)(x) & 0xff000000) == 0xc0000000) ||             \
                        (((u32)(x) & 0xff800000) == 0xc1000000) ||             \
                        (((u32)(x) & 0xf8000000) == 0xd0000000) ||             \
                        (((u32)(x) & 0xffffc000) == 0xe0000000),               \
                        line,                                                  \
                    "NW4R:Pointer Error\n" #x "(=%p) is not valid pointer.", (x))

void VPanic(const char* file, int line, const char* fmt, va_list vlist, bool halt);
void Panic(const char* file, int line, const char* fmt, ...);
void VWarning(const char* file, int line, const char* fmt, va_list vlist);
void Warning(const char* file, int line, const char* msg, ...);

detail::ConsoleHead* Assertion_SetConsole(detail::ConsoleHead* console);
detail::ConsoleHead* Assertion_GetConsole();
void Assertion_ShowConsole(u32 time);
void Assertion_HideConsole();
void Assertion_SetWarningTime(u32 time);
bool Assertion_SetAutoWarning(bool enable);
		

} // namespace db
} // namespace nw4r

#endif