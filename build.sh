#!/bin/bash
./autogen.sh
./configure --prefix=`pwd` --enable-static=no --with-axis2=${AXIS2C_HOME}/include
make
make install
