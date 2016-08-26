#!/bin/sh

cd nDPI
NDPI_DIR=$(pwd)
echo $NDPI_DIR
NDPI_INCLUDE=$(pwd)/src/include
echo $NDPI_INCLUDE
./autogen.sh
./configure
make
make install

#Build PF_RING library
cd ..
cd PF_RING
make
#Build PF_RING examples, including the modified pfbridge, with nDPI integrated.
cd userland/examples/
sed -i 's#EXTRA_LIBS =#EXTRA_LIBS='"${NDPI_DIR}"'/src/lib/.libs/libndpi.a -ljson-c -lcurl#' ./Makefile
sed -i 's# -Ithird-party# -Ithird-party/ -I'"$NDPI_INCLUDE"' -I'"$NDPI_DIR"'#' ./Makefile
echo $NDPI_DIR
make
