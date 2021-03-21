#pragma once

#include <stdbool.h>

struct tests_report;

int  tests_run    ( int argc, char * argv[], ... );
void tests_chapter( struct tests_report * report, const char * title );
void tests_assert ( struct tests_report * report, const char * test, bool test_result, const char * file, int line );
void tests_ended  ( struct tests_report * report );

#define ASSERT( C, T ) tests_assert( C, #T, T, __FILE__, __LINE__ )
