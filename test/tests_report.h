#pragma once

#include <stdbool.h>

struct tests_report;

int  tests_run    ( int argc, char * argv[], ... );
void tests_chapter( struct tests_report * report, const char * title );
void tests_assert ( struct tests_report * report, const char * test, bool observed, bool expected );
void tests_ended  ( struct tests_report * report );

#define ASSERT( C, T, E ) tests_assert( C, #T, T, E )
