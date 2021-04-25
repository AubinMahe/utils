#include <tst/tests_report.h>
#include <tst/iso6429.h>

#include <execinfo.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define HEAVY_CHECK_MARK      "\u2714"
#define HEAVY_BALLOT_X        "\u2718"

#define TEST_PASSED           ISO_6429_FG_GREEN HEAVY_CHECK_MARK ISO_6429_RESET
#define TEST_FAILED           ISO_6429_FG_RED   HEAVY_BALLOT_X   ISO_6429_RESET

typedef void ( * tests_chapter_func )( struct tests_report * ctxt );

typedef struct tests_chapter_s {
   const char *             name;
   tests_chapter_func       func;
   bool                     selected;
   struct tests_chapter_s * next;
} tests_chapter_t;

struct tests_report {
   pthread_mutex_t   lock;
   tests_chapter_t * tests;
   unsigned          passed;
   unsigned          failed;
   unsigned          sum_passed;
   unsigned          sum_failed;
};

const struct tests_report tests_Zero = { PTHREAD_MUTEX_INITIALIZER, NULL, 0, 0, 0, 0 };

DLL_PUBLIC void tests_chapter( struct tests_report * ctxt, const char * title ) {
   pthread_mutex_lock( &ctxt->lock );
   if( ctxt->failed || ctxt->passed || ctxt->sum_failed || ctxt->sum_passed ) {
      ctxt->sum_failed += ctxt->failed;
      ctxt->sum_passed += ctxt->passed;
      if( ctxt->failed == 0 ) {
         printf( "   %d tests, %sall successful%s\n", ctxt->passed, ISO_6429_FG_GREEN, ISO_6429_RESET );
      }
      else if( ctxt->passed == 0 ) {
         printf( "   %d tests, %sall failed%s\n", ctxt->failed, ISO_6429_FG_RED, ISO_6429_RESET );
      }
      else if( ctxt->failed > 0 ) {
         printf( "   %d tests, %s%d successful%s, %s%d failed%s\n", ctxt->passed + ctxt->failed,
            ISO_6429_FG_GREEN, ctxt->passed, ISO_6429_RESET,
            ISO_6429_FG_RED  , ctxt->failed, ISO_6429_RESET );
      }
   }
   ctxt->failed = 0;
   ctxt->passed = 0;
   if( title ) {
      printf( "\n%s%s%s\n\n", ISO_6429_BOLD, title, ISO_6429_RESET );
   }
   pthread_mutex_unlock( &ctxt->lock );
}

DLL_PUBLIC void tests_assert( struct tests_report * ctxt, const char * test, bool test_result, const char * file, int line ) {
   pthread_mutex_lock( &ctxt->lock );
   if( test_result ) {
      printf( "   %s %s\n", TEST_PASSED, test );
      ++(ctxt->passed);
   }
   else {
      printf( "   %s %s\t\t(%s:%d)\n", TEST_FAILED, test, file, line );
      ++(ctxt->failed);
   }
   pthread_mutex_unlock( &ctxt->lock );
}

DLL_PUBLIC int tests_run( int argc, char * argv[], ... ) {
   struct tests_report This = tests_Zero;
   va_list tsts;
   va_start( tsts, argv );
   const char * testname;
   while(( testname = va_arg( tsts, const char * )) != NULL ) {
      tests_chapter_t * chapter = malloc( sizeof( tests_chapter_t ));
      chapter->name     = testname;
      chapter->func     = va_arg( tsts, tests_chapter_func );
      chapter->selected = ( argc == 1 );
      chapter->next     = This.tests;
      This.tests = chapter;
   }
   va_end( tsts );
   for( int i = 1; i < argc; ++i ) {
      const char * arg = argv[i];
      tests_chapter_t * chapter = This.tests;
      bool found = false;
      while( chapter && ! found ) {
         if( 0 == strcmp( chapter->name, arg )) {
            chapter->selected = true;
            found = true;
         }
         chapter = chapter->next;
      }
      if( ! found ) {
         fprintf( stderr, "Unexpected argument: %s\n", arg );
         fprintf( stderr, "Valid tests name are:\n" );
         chapter = This.tests;
         while( chapter ) {
            fprintf( stderr, "\t%s\n", chapter->name );
            chapter = chapter->next;
         }
         return EXIT_FAILURE;
      }
   }
   tests_chapter_t * chapter = This.tests;
   while( chapter ) {
      if( chapter->selected ) {
         chapter->func( &This );
      }
      chapter = chapter->next;
   }
   tests_chapter( &This, NULL );
   if( This.sum_failed == 0 ) {
      printf( "\n%s%d tests, %sall successful%s\n", ISO_6429_BOLD, This.sum_passed, ISO_6429_FG_GREEN, ISO_6429_RESET );
   }
   else if( This.sum_passed == 0 ) {
      printf( "\n%s%d tests, %sall failed%s\n", ISO_6429_BOLD, This.sum_failed, ISO_6429_FG_RED, ISO_6429_RESET );
   }
   else if( This.sum_failed > 0 ) {
      printf( "\n%s%d tests, %s%d successful%s, %s%d failed%s\n", ISO_6429_BOLD, This.sum_passed + This.sum_failed,
         ISO_6429_FG_GREEN, This.sum_passed, ISO_6429_RESET,
         ISO_6429_FG_RED  , This.sum_failed, ISO_6429_RESET );
   }
   chapter = This.tests;
   while( chapter ) {
      tests_chapter_t * next = chapter->next;
      free( chapter );
      chapter = next;
   }
   return EXIT_SUCCESS;
}
