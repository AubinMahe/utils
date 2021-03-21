CFLAGS = -pthread -std=c11 -I inc -fvisibility=hidden -D_DEFAULT_SOURCE=__STRICT_ANSI__ -D_FORTIFY_SOURCE=2 -fPIC\
 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wconversion -Wcast-align -Wcast-qual -Wdisabled-optimization -Wlogical-op\
 -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wundef\
 -Wwrite-strings -Wfloat-equal -fmessage-length=0

SRCS =\
 src/net/net_buff.c\
 src/utils/utils_cli.c\
 src/utils/utils_map.c\
 src/utils/utils_prefs.c\
 src/utils/utils_set.c

SRCS_TST =\
 test/main.c\
 test/tests_report.c\
 test/net_buff_test.c\
 test/utils_cli_test.c\
 test/utils_map_test.c\
 test/utils_prefs_test.c\
 test/utils_set_test.c

OBJS         = $(SRCS:%c=BUILD/%o)
OBJS_DBG     = $(SRCS:%c=BUILD/DEBUG/%o)
OBJS_DBG_TST = $(SRCS_TST:%c=BUILD/DEBUG/%o)
DEPS         = $(SRCS:%c=BUILD/%d)
DEPS_TST     = $(SRCS_TST:%c=BUILD/%d)

.PHONY: all
all: libutils-d.so libutils.so tests-d

.PHONY: check-make
check-make:
	@echo "DEPS    : $(DEPS)"
	@echo "DEPS_TST: $(DEPS_TST)"

.PHONY: validate
validate: tests-d
	@g++ test/cplusplus_ckeck.cpp -c -I inc -W -Wall -pedantic
	@rm cplusplus_ckeck.o
	@LD_LIBRARY_PATH=. ./tests-d

.PHONY: validate-valgrind
validate-valgrind: tests-d
	LD_LIBRARY_PATH=. valgrind --leak-check=full --show-leak-kinds=all ./tests-d

.PHONY: clean
clean:
	rm -fr BUILD bin depcache build Debug Release
	rm -f libutils-d.so libutils.so tests-d

libutils.so: $(OBJS)
	gcc $^ -shared -o $@
	strip --discard-all --discard-locals $@

libutils-d.so: $(OBJS_DBG)
	gcc $^ -shared -o $@

tests-d: $(OBJS_DBG_TST) libutils-d.so
	gcc $(OBJS_DBG_TST) -o $@ -pthread -L. -lutils-d

BUILD/%.o: %.c
	@mkdir -p $$(dirname $@)
	gcc $(CFLAGS) -O3 -g0 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

BUILD/DEBUG/%.o: %.c
	@mkdir -p $$(dirname $@)
	gcc $(CFLAGS) -O0 -g3 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

-include $(DEPS) $(DEPS_TST)
