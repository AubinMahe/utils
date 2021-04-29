CC        := gcc
CXX       := g++
CC_WIN64  := x86_64-w64-mingw32-gcc
CXX_WIN64 := x86_64-w64-mingw32-g++

CFLAGS    := -pthread -std=c11 -I inc -fvisibility=hidden -D_DEFAULT_SOURCE=__STRICT_ANSI__ -D_FORTIFY_SOURCE=2\
 -Wall -Wextra -Werror -Wconversion -Wcast-align -Wcast-qual -Wdisabled-optimization -Wlogical-op\
 -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wundef\
 -Wwrite-strings -Wfloat-equal -fmessage-length=0

SRCS :=\
 src/net/net_buff.c\
 src/tst/tests_report.c\
 src/utils/utils_cli.c\
 src/utils/utils_map.c\
 src/utils/utils_prefs.c\
 src/utils/utils_set.c\
 src/utils/utils_time.c

SRCS_TST :=\
 test/main.c\
 test/net_buff_test.c\
 test/utils_cli_test.c\
 test/utils_map_test.c\
 test/utils_prefs_test.c\
 test/utils_set_test.c\
 test/utils_time_test.c

OBJS               := $(SRCS:%c=BUILD/%o)
OBJS_WIN64         := $(SRCS:%c=BUILD/WIN64/%o)
OBJS_DBG           := $(SRCS:%c=BUILD/DEBUG/%o)
OBJS_DBG_WIN64     := $(SRCS:%c=BUILD/WIN64/DEBUG/%o)
OBJS_TST_DBG       := $(SRCS_TST:%c=BUILD/DEBUG/%o)
OBJS_TST_DBG_WIN64 := $(SRCS_TST:%c=BUILD/WIN64/DEBUG/%o)
DEPS               := $(SRCS:%c=BUILD/%d)
DEPS_TST           := $(SRCS_TST:%c=BUILD/%d)

VALGRIND_OPTIONS :=\
 --leak-check=full\
 --show-leak-kinds=all\
 --track-origins=yes\
 --track-fds=yes\
 --error-exitcode=2\
 --expensive-definedness-checks=yes

.PHONY: all check-make validate memcheck map clean

all: libutils.so libutils-d.so tests-d utils.dll utils-d.dll tests-d tests-d.exe

check-make:
	@echo "DEPS    : $(DEPS)"
	@echo "DEPS_TST: $(DEPS_TST)"

validate: tests-d
	@$(CXX) test/cplusplus_ckeck.cpp -c -I inc -W -Wall -pedantic
	@rm cplusplus_ckeck.o
	@LD_LIBRARY_PATH=. ./tests-d

validate-win64: tests-d.exe
	@$(CXX_WIN64) test/cplusplus_ckeck.cpp -c -I inc -W -Wall -pedantic
	@rm cplusplus_ckeck.o
	@LC_ALL=fr_FR.UTF-8 wine ./tests-d.exe

memcheck: tests-d
	LD_LIBRARY_PATH=. valgrind $(VALGRIND_OPTIONS) ./tests-d

map: tests-d
	LD_LIBRARY_PATH=. valgrind $(VALGRIND_OPTIONS) ./tests-d utils_map

clean:
	rm -fr BUILD bin depcache build Debug Release
	rm -f libutils-d.so libutils.so tests-d

libutils.so: $(OBJS)
	$(CC) $^ -shared -o $@
	strip --discard-all --discard-locals $@

libutils-d.so: $(OBJS_DBG)
	$(CC) $^ -shared -o $@

tests-d: $(OBJS_TST_DBG) libutils-d.so
	$(CC) $(OBJS_TST_DBG) -o $@ -pthread -L. -lutils-d

utils.dll: $(OBJS_WIN64)
	$(CC_WIN64) $^ -shared -static -o $@ -Wl,--out-implib=libutils.a -lpthread -lws2_32
	strip --discard-all --discard-locals $@

utils-d.dll: $(OBJS_DBG_WIN64)
	$(CC_WIN64) $^ -shared -static -o $@ -Wl,--out-implib=libutils-d.a -lpthread -lws2_32

tests-d.exe: $(OBJS_TST_DBG_WIN64) utils-d.dll
	$(CC_WIN64) $(OBJS_TST_DBG_WIN64) -static -Wl,--subsystem,console -mconsole -o $@ -lpthread -lws2_32 -L. -lutils-d

BUILD/%.o: %.c
	@mkdir -p $$(dirname $@)
	$(CC) $(CFLAGS) -pedantic -pedantic-errors -fpic -O3 -g0 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

BUILD/DEBUG/%.o: %.c
	@mkdir -p $$(dirname $@)
	$(CC) $(CFLAGS) -pedantic -pedantic-errors -fpic -O0 -g3 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

BUILD/WIN64/src/%.o: src/%.c
	@mkdir -p $$(dirname $@)
	$(CC_WIN64) $(CFLAGS) -DBUILDING_DLL -O3 -g0 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

BUILD/WIN64/DEBUG/src/%.o: src/%.c
	@mkdir -p $$(dirname $@)
	$(CC_WIN64) $(CFLAGS) -DBUILDING_DLL -O0 -g3 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

BUILD/WIN64/DEBUG/test/%.o: test/%.c
	@mkdir -p $$(dirname $@)
	$(CC_WIN64) $(CFLAGS) -O0 -g3 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

-include $(DEPS) $(DEPS_TST)
