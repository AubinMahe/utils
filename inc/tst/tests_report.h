#pragma once

#include <stdbool.h>

#include <utils/utils_visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tests_report;

DLL_PUBLIC void tests_chapter( struct tests_report * report, const char * title );
DLL_PUBLIC void tests_assert ( struct tests_report * report, const char * test, bool test_result, const char * file, int line );
DLL_PUBLIC int  tests_run    ( int argc, char * argv[], ... );

#define ASSERT( C, T ) tests_assert( C, #T, T, __FILE__, __LINE__ )

#ifdef __cplusplus
}
#endif
