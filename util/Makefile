################################################################################
#
#  Makefile for hpiutils
#
################################################################################

# All items listed will be linked.  By default, each has a dependency of the corresponding .o file.
# Each .o file has a default dependency of the correspoonding .c file.
TARGETS = hpialarmpanel hpisensor hpisel hpireset hpiwdt hpifru hpievent

ifeq ($(DEBUG),y)
DEBFLAGS = -O -g3 -DDEBUG
else
DEBFLAGS = -O2
endif
CFLAGS  = $(DEBFLAGS) -Wall -D_REENTRANT  -DLINUX -DUNIX -DUWSVR4 

# OpenHPI parameters
LIBDIR = /usr/local/lib
LIBHPI  = openhpi
# Intel HPI parameters (uncomment the next 2 lines)
#LIBDIR = /usr/lib
#LIBHPI  = SaHpi

# Additional compile flags
CC = gcc
CFLAGS +=
LIBHPIF = $(LIBDIR)/lib$(LIBHPI).so

# Link flags
LDFLAGS  = -pthread
LDFLAGS += -lc
LDFLAGS += -lm
LDFLAGS += -L $(LIBDIR)
LD_LIBRARY_PATH = $(LIBDIR)

all:    $(TARGETS)

hpialarmpanel:  hpialarmpanel.o $(LIBHPIF)
	$(CC) $(LDFLAGS) -l$(LIBHPI) -o hpialarmpanel hpialarmpanel.o 

hpisensor:  hpisensor.o  $(LIBHPIF)
	$(CC) $(LDFLAGS) -l$(LIBHPI) -o hpisensor hpisensor.o 

hpisel:  hpisel.o  $(LIBHPIF)
	$(CC) $(LDFLAGS) -l$(LIBHPI) -o hpisel hpisel.o 

hpifru:  hpifru.o  $(LIBHPIF)
	$(CC) $(LDFLAGS) -l$(LIBHPI) -o hpifru hpifru.o 

hpireset:  hpireset.o  $(LIBHPIF)
	$(CC) $(LDFLAGS) -l$(LIBHPI) -o hpireset hpireset.o 

hpiwdt:  hpiwdt.o  $(LIBHPIF)
	$(CC) $(LDFLAGS) -l$(LIBHPI) -o hpiwdt hpiwdt.o 

hpievent:  hpievent.o  $(LIBHPIF)
	$(CC) $(LDFLAGS) -l$(LIBHPI) -o hpievent hpievent.o 

clean:
	rm -f *.o *.i *.s $(TARGETS) *.rpm

distclean: clean
	rm -f Makefile.deps

