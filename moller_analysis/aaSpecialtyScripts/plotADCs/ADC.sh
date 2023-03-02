#!/bin/bash
cd ~/halla_molana/moller_analysis
run=$1
if [ $# -ne 1 ];
then
   echo "Requires 1 argument: run number"
else
    root -l plotADCs.C+\($1\)
fi

cd -
