OBJDIR := build
EXECUTABLE := aaoss
CLEANTARGETS := $(OBJDIR) $(EXECUTABLE)

SRCDIR := src
INCDIR := include

SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(SRC:$(SRCDIR)/%.c=%.o) lex.lex.o parse.tab.o

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

override CFLAGS += -I $(INCDIR) -MMD -MP -MF $(OBJDIR)/$(notdir $*).d -std=c11

# near-magic echo taken from Linux's Makefile
# echo all commands if $V is set
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

$(EXECUTABLE): % : $(addprefix $(OBJDIR)/,$(OBJ)) $(OBJDIR)/%.o
	$(call cmd,link_o_target)

quiet_cmd_cc_o_c = CC		$@
cmd_cc_o_c = $(COMPILE.c) $(OUTPUT_OPTION) $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(call cmd,cc_o_c)

%.o: %.c | $(OBJDIR)
	$(call cmd,cc_o_c)

quiet_cmd_bison = YACC		$(basename $@).[ch]
cmd_bison = $(YACC) -o $(basename $@).c --defines=$(basename $@).h -l $<

$(OBJDIR)/%.tab.c $(OBJDIR)/%.tab.h: $(SRCDIR)/%.y | $(OBJDIR)
	$(call cmd,bison)

quiet_cmd_flex = LEX		$@
cmd_flex = $(LEX.l) $< > $@

$(OBJDIR)/%.lex.c: $(SRCDIR)/%.l | $(OBJDIR)
	$(call cmd,flex)

# dependencies on generated files need to be listed explicitly
$(OBJDIR)/lex.lex.o: $(OBJDIR)/parse.tab.h

-include $(OBJDIR)/$(SRC:$(SRCDIR)/%.c=%.d)
