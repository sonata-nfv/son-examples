#
# Makefile for both static and dynamic PF_RING user libraries
#
#

#
# Installation directory
#
INSTDIR   = ${DESTDIR}//usr/local

#
# Search directories
#
INCLUDE   = -I../../kernel -I../libpcap

#
# Zero ToolKit
#
ZERO_OBJS  = pfring_dna_bouncer.o pfring_mod_dna_cluster.o pfring_dna_cluster.o pfring_dna_vas.o pfring_dna_sal.o pfring_libzero.o hugepages_lib.o 

#
# DNA
#
DNA_OBJS  = pfring_mod_dna.o pfring_dna.o pfring_e1000e_dna.o pfring_e1000_dna.o pfring_ixgbe_dna.o pfring_igb_dna.o silicom_ts.o pfring_dna_utils.o 

#
# PF_RING ZC
#
PF_RING_ZC_OBJS  = pfring_zc_dev_e1000.o pfring_zc_dev_e1000e.o pfring_zc_dev_ixgbe.o pfring_zc_dev_igb.o pfring_zc_dev_i40e.o pfring_zc_dev_rss.o pfring_zc_dev_sal.o pfring_mod_zc.o pfring_mod_zc_dev.o pfring_mod_zc_spsc.o pfring_zc_cluster.o pfring_zc_mm.o uio_lib.o hugetlb_lib.o pfring_zc_kvm.o pfring_zc_kvm_utils.o silicom_ts_card.o 

#
# DAG
#
DAG_OBJS  = 

#
# NT
#
NT_OBJS  = 

#
# Mellanox (MLX)
#
MLX_OBJS  = 

#
# Accolade
#
ACCOLADE_OBJS  = 

#
# Object files
#
OBJS      = pfring.o pfring_mod.o pfring_utils.o pfring_mod_stack.o pfring_hw_filtering.o \
	    pfring_hw_timestamp.o pfring_mod_sysdig.o ${ZERO_OBJS} ${DNA_OBJS} ${PF_RING_ZC_OBJS} ${DAG_OBJS} ${NT_OBJS} ${MLX_OBJS} ${ACCOLADE_OBJS}

#
# C compiler and flags
#
#
# CROSS_COMPILE=arm-mv5sft-linux-gnueabi-
#
CC        = ${CROSS_COMPILE}gcc
CFLAGS    =  -march=corei7 -mtune=corei7  -Wall -fPIC ${INCLUDE} -D HAVE_ZERO -D HAVE_DNA -D HAVE_PF_RING_ZC     -D ENABLE_BPF  -D ENABLE_HW_TIMESTAMP -D HAVE_LIBNUMA   -O2 # -g
LDFLAGS   = -g -shared
SYSLIBS   = -lpthread  -lrt  

# How to make an object file
%.o: %.c %.h Makefile
#	@echo "=*= making object $@ =*="
	${CC} ${CFLAGS} -c $< -o $@

#
# Main targets
#
STATICLIB  = libpfring.a
DYNAMICLIB = libpfring.so
TARGETS    = ${STATICLIB} ${DYNAMICLIB}
RING_H     = ../../kernel/linux/pf_ring.h

all: ${TARGETS}

${STATICLIB}: Makefile extract_zero_lib extract_dna_lib extract_pfring_zc_lib  ${OBJS} pfring.h ${RING_H}
	@echo "=*= making library $@ =*="
	ar rs $@ ${OBJS}
	ranlib $@

${DYNAMICLIB}: ${OBJS} extract_zero_lib extract_dna_lib extract_pfring_zc_lib  pfring.h ${RING_H} Makefile
	@echo "=*= making library $@ =*="
	${CC} ${LDFLAGS} ${OBJS} ${SYSLIBS} -o $@

extract_dna_lib:
	ar x libs/libpfring_dna_x86_64_corei7.a

extract_zero_lib:
	ar x libs/libpfring_zero_x86_64_corei7.a

extract_pfring_zc_lib:
	ar x libs/libpfring_zc_x86_64_corei7.a

extract_nt_lib:
	ar x libs/libpfring_nt_x86_64_corei7.a

install: install-static install-shared

install-includes:
	mkdir -p ${INSTDIR}/include
	cp pfring.h pfring_mod_sysdig.h pfring_zc.h ${INSTDIR}/include/

install-static: ${STATICLIB} install-includes
	mkdir -p ${INSTDIR}/lib
	cp ${STATICLIB} ${INSTDIR}/lib/

install-shared:	${DYNAMICLIB} install-includes
	mkdir -p ${INSTDIR}/lib
	cp ${DYNAMICLIB} ${INSTDIR}/lib/
	@if test "$(USER)" = "root"; then \
		ldconfig; \
	fi

clean:
	@rm -f ${TARGETS} *.o *~
