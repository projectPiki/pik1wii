#ifndef EGG_PRIM_ASSERT_H
#define EGG_PRIM_ASSERT_H

#include <egg/egg_types.h>

#ifdef EGG_LOG
#define EGG_PRINT(...) system_print(true, __FILE__, __VA_ARGS__)

#define EGG_ASSERT(LINE, EXP)                                                        \
    if (!(EXP))                                                                \
    system_halt(__FILE__, LINE, #EXP)

#define EGG_ASSERT_MSG(LINE, EXP, ...)                                               \
    if (!(EXP))                                                                \
    system_halt(__FILE__, LINE, __VA_ARGS__)
#else
#define EGG_PRINT(...) ((void)(0))
#define EGG_ASSERT(EXP) ((void)(0))
#define EGG_ASSERT_MSG(EXP, ...) ((void)(0))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(NONMATCHING) || defined(COMPAT_ANY)
void system_print(bool visible, const char* pFile, const char* pMsg, ...);
#else
void system_print(bool visible, char* pFile, char* pMsg, ...);
#endif

#if defined(NONMATCHING) || defined(COMPAT_ANY)
void system_halt(const char* pFile, int line, const char* pMsg, ...);
#else
void system_halt(char* pFile, int line, char* pMsg, ...);
#endif

#ifdef __cplusplus
}
#endif
#endif
