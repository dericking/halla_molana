#!/bin/bash
run=$1
if [ $# -ne 1 ]; then
   echo "Requires 1 argument: run number"
else
    root -l plotBCM.C+\($1\)
fi


