OBJDIR := build
EXECUTABLE := aaoss
CLEANTARGETS := $(OBJDIR) $(EXECUTABLE)

SRCDIR := src
INCDIR := include

SRC := $(filter-out $(EXECUTABLE:%=$(SRCDIR)/%.c),$(wildcard $(SRCDIR)/*.c))
OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

LDLIBS := -lstdc++
#XXX LDFLAGS ?= -Wl,-O1 -Wl,--as-needed

# one of address, thread, memory
SANITIZE ?= address

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
	override CC := clang
	override CFLAGS := -O1 -g -Weverything -fno-omit-frame-pointer -fsanitize=$(SANITIZE),undefined
	override LDFLAGS += -fuse-ld=lld
else
	CFLAGS ?= -O2 -march=native -pipe
endif

override CFLAGS += -I $(INCDIR) -MMD -MP -MF $(OBJDIR)/$*.d -std=c11

# near-magic echo taken from Linux's Makefile
# echo all commands if $V is set, replacing echo commands with "true"
ifeq ($(V),)
	quiet := quiet_
endif

# suppress echo of commands if in silent mode (-s)
ifneq ($(findstring s,$(filter-out --%,$(MAKEFLAGS))),)
	quiet := silent_
endif

squote := '
escsq = $(subst $(squote),'\$(squote)',$1)
echo-cmd = $(if $($(quiet)cmd_$(1)), echo '  $(call escsq,$($(quiet)cmd_$(1)))';)
cmd = @$(echo-cmd) $(cmd_$(1))

.PHONY: all help debug valgrind gdb clean run coverage
.PRECIOUS: $(OBJDIR)/%.d

all: $(EXECUTABLE)	## build all executables

debug: clean all	## build with extra debugging information
	-clang-tidy $(SRC)

valgrind: debug	## rebuild and execute valgrind
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXECUTABLE)

gdb: debug	## rebuild and execute gdb
	gdb $(EXECUTABLE)

help:	## this help
	@echo "make [-j4] [TARGET]...	where TARGET is one or more of:"
	@awk -F ':.*?## ' '/^[a-zA-Z]/ && NF==2 {printf "  %-26s%s\n", $$1, $$2}' $(MAKEFILE_LIST) | sort

quiet_cmd_clean = CLEAN		$(CLEANTARGETS)
cmd_clean = rm -rf $(CLEANTARGETS)

clean:	## remove all build artifacts
	$(call cmd,clean)

quiet_cmd_mkdir = MKDIR		$@
cmd_mkdir = mkdir -p $@

$(OBJDIR): $(filter clean,$(MAKECMDGOALS))
	$(call cmd,mkdir)

run: all	## run the executable
	@./$(EXECUTABLE)

quiet_cmd_link_o_target = LD		$@
cmd_link_o_target = $(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(EXECUTABLE): % : $(OBJ) $(OBJDIR)/%.o
	$(call cmd,link_o_target)

quiet_cmd_cc_o_c = CC		$@
cmd_cc_o_c = $(COMPILE.c) $(OUTPUT_OPTION) $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(call cmd,cc_o_c)

include $(wildcard $(patsubst %,$(OBJDIR)/%.d,$(basename $(SRC))))
