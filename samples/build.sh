#!/bin/bash

./autogen.sh

./configure --prefix=${AXIS2C_HOME} --with-axis2=${AXIS2C_HOME}/include/axis2-1.4.0
make 
make install
