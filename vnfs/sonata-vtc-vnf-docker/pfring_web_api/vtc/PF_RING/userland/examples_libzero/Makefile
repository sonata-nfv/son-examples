#
# Makefile for the PF_RING API example programs
#
# Try to make both programs written on top of the
# PF_RING library and programs originally written
# on top of PCAP library and linked now to use
# the version with PF_RING aware functionalities.
#



#
# PF_RING
#
PFRINGDIR  = ../lib
LIBPFRING  = ${PFRINGDIR}/libpfring.a

#
# DNA Support
DNA_DEFINE = #-DENABLE_DNA_SUPPORT

#
# PF_RING aware libpcap
#
O_FLAG     = -O2 -DHAVE_PF_RING
EXTRA_LIBS =  -lrt -lnuma
PCAPDIR    = ../libpcap
LIBPCAP    = ${PCAPDIR}/libpcap.a  ${EXTRA_LIBS}

#
# Search directories
#
PFRING_KERNEL=../../kernel
INCLUDE    = -I${PFRING_KERNEL} -I${PFRING_KERNEL}/plugins -I${PFRINGDIR} -I${PCAPDIR} -I../examples

#
# C compiler and flags
#
#
# CROSS_COMPILE=arm-mv5sft-linux-gnueabi-
#
CC         = ${CROSS_COMPILE}gcc #--platform=native
CFLAGS     = -march=corei7 -mtune=corei7  ${O_FLAG} -Wall ${INCLUDE} ${DNA_DEFINE} -D HAVE_ZERO -D ENABLE_BPF -D HAVE_LIBNUMA -O2 # -g
# LDFLAGS  =

#
# User and System libraries
#
LIBS       = ${LIBPCAP} ${LIBPFRING}   -lpthread  -lrt ${LIBPCAP}   -lnuma -lrt

# How to make an object file
%.o: %.c pfutils.c
#	@echo "=*= making object $@ =*="
	${CC} ${CFLAGS} -c $< -o $@

#
# Main targets
#
PFPROGS   = 

ifneq (-D HAVE_ZERO,)
	PFPROGS += pfdnabounce pfdnacluster_master pfdnacluster_multithread pfdnacluster_mt_rss_frwd
endif

TARGETS   = ${PFPROGS}

all: ${TARGETS}

pfdnabounce: pfdnabounce.o ${LIBPFRING}
	${CC} ${CFLAGS} pfdnabounce.o ${LIBS} -o $@

pfdnacluster_master: pfdnacluster_master.o ${LIBPFRING}
	${CC} ${CFLAGS} pfdnacluster_master.o ${LIBS} -o $@

pfdnacluster_multithread: pfdnacluster_multithread.o ${LIBPFRING}
	${CC} ${CFLAGS} pfdnacluster_multithread.o ${LIBS} -o $@

pfdnacluster_mt_rss_frwd: pfdnacluster_mt_rss_frwd.o ${LIBPFRING}
	${CC} ${CFLAGS} pfdnacluster_mt_rss_frwd.o ${LIBS} -o $@

clean:
	@rm -f ${TARGETS} *.o *~
