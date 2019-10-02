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
	rm -r liblitl -f

litl: liblitl liblitl_sem_tcp_ts_mutlock liblitl_sem_tcp_ts_fifo_mutlock liblitl_sem_tcp_mutlock liblitl_sem_tcp_fifo_mutlock liblitl_sem_tcp2_mutlock liblitl-patch
	cd liblitl; make

liblitl:
	@echo "---- Cloning litl framework ----"
	git clone https://github.com/multicore-locks/litl.git liblitl 

liblitl-patch: liblitl/liblitl-patch

liblitl/liblitl-patch:
	@echo "---- Apply patch to litl framework ----"
	-cd liblitl; patch -p1 < ../liblitl-patch;
	cp liblitl-patch liblitl/liblitl-patch

liblitl_%:
	@echo "---- Copy and patch $(patsubst liblitl_%, %, $@) from libmutlock to litl framework ----"
	cp $(SRCDIR)/utils/spin_utils.h liblitl/include
	cp $(SRCDIR)/utils/sleep_utils.h liblitl/include
	cp $(SRCDIR)/queued-spinlock/queued_spinlock.h liblitl/include
	cp $(SRCDIR)/utils/node_basket.h liblitl/include
	cp $(SRCDIR)/mutlocks/mutlocks_common.h liblitl/include
	cp $(SRCDIR)/mutlocks/mutlocks_template.h liblitl/include
	cp $(SRCDIR)/mutlocks/mutlocks_template_ending.h liblitl/include
	cp $(SRCDIR)/mutlocks/$(patsubst liblitl_%,%, $@).h liblitl/include
	cp $(SRCDIR)/mutlocks/$(patsubst liblitl_%,%, $@).c liblitl/src
	mv liblitl/include/$(patsubst liblitl_%,%, $@).h liblitl/include/$(subst _,,$(patsubst liblitl_%,%, $@)).h
	mv liblitl/src/$(patsubst liblitl_%,%, $@).c liblitl/src/$(subst _,,$(patsubst liblitl_%,%, $@)).c