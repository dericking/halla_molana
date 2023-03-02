#!/bin/bash
cd ~/halla_molana/moller_analysis
run=$1
if [ $# -ne 1 ];
then
   echo "Requires 1 argument: run number"
else
	./run_molana_analysis.sh -f -r $1 
	wait;
	cd aaSpecialtyScripts/hv_tuning
    root -l csv_creator.C+'('$1')'
	wait;
	python3 ./hv_tuner.py
fi

cd -
