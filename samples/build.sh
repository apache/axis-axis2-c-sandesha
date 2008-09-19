#!/bin/bash
POLICY_DIR="$AXIS2C_HOME/bin/samples/sandesha2/policy"

./autogen.sh

./configure --prefix=${AXIS2C_HOME} --with-axis2=${AXIS2C_HOME}/include/axis2-1.5.0
make -j30
make install

if [ -d  $POLICY_DIR];
then
    echo "$POLICY_DIR exists. "
else
    #Create Policy Dir
    echo "Deploying Policies"
    mkdir $POLICY_DIR
fi

cp policy/* $POLICY_DIR/
