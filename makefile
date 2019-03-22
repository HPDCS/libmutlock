CXX_OPT:=-std=gnu99 -fPIC

# COMPILER
CXX:=gcc

# SHARED OBJECTS
LIBS:=-lpthread

SRCDIR:=./src

SUBDIRS:= \
	$(SRCDIR)/cas-spinlock \
	$(SRCDIR)/tas-spinlock \
	$(SRCDIR)/queued-spinlock \
	$(SRCDIR)/mutable-lock \
	$(SRCDIR)/utils


#	$(SRCDIR)/heuristic-mutlocks \
		
OPT:= -O2

LFLAGS:=

LPATH:=

SOURCES:= $(shell find $(SRCDIR) -name '*.c')

HEADERS:= $(shell find $(SRCDIR) -name '*.h')

INCLUDES:= $(addprefix -I,$(SUBDIRS))

OBJ:=$(SOURCES:.c=.o)

DEBUG:= -g3

MACROS+= #-DMUTLOCK_DEBUG

WARNINGS:= -Wall #-Wno-discarded-qualifiers #-Wno-unused-value

TARGETS:=libmutlock.so
SHARED_OPT:=-shared -Wl,-soname,$(TARGETS)


C_DEPS	:= $(OBJ:.o=.d)
C_DEPS 	:= $(patsubst ./src/%, ./deps_src/%, $(C_DEPS))

INSTALL_SO_PATH?=libs
INSTALL_INC_PATH?=include

all: $(TARGETS)

install: 
	mkdir -p $(INSTALL_SO_PATH); mkdir -p $(INSTALL_INC_PATH)
	cp $(TARGETS) $(INSTALL_SO_PATH)
	cp $(HEADERS) $(INSTALL_INC_PATH)
	
$(TARGETS): $(OBJ)
	@echo "---- Generating target ----"
	$(CXX) $(CXX_OPT) $(SHARED_OPT) $(LFLAGS) $(LPATH) $(OBJ) -o $@  $(LIBS)
	@echo "---- Target successfully generated ----"

-include $(C_DEPS)
		
%.o: %.c
	@mkdir -p $(dir $(patsubst src/%,deps_src/%, $<))
	@echo "---- Building source file" $<" ----"
	@echo "---- Generating binary file" $@ " ----"
	$(CXX) $(CXX_OPT) $(WARNINGS) $(DEBUG) $(MACROS) $(OPT) $(INCLUDES) $(OTHERFLAGS) -MMD -MP -MF"$(patsubst src/%,deps_src/%, $(@:%.o=%.d))" -MT"$(@)" -c $< -o $@
	@echo "---- Done ----"

clean:
	rm -rf $(OBJ) $(TARGETS)  $(OBJ:.o=.d) deps_src
