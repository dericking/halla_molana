#!/bin/bash

TARGPOL=0.08005;

STARTGRP=$1;

##COPY LARGER ENV VAR NAMES TO COMPACT
MDBHOST=${MOLANA_DB_HOST}
MDBUSER=${MOLANA_DB_USER}
MDBPASS=${MOLANA_DB_PASS}
MDBNAME=${MOLANA_DB_NAME}
MROOTDR=${MOLLER_ROOTFILE_DIR}
ANALDIR=${MOLANA_DATADECODER_DIR}
RSLTDIR=${MOLANA_ONLINE_PUSH_DIR}

rm -f BURSTLIST.txt

[ -d "${RSLTDIR}/burst" ] && echo "run_molana_analysis() ==> Specified analysis files directory exists." || mkdir "${RSLTDIR}/burst"

## WE CAN SPECIFY A SINGLE GROUP OR LET IT AUTOMATICALLY POPULATE ALL GROUPS
if [[ -z "${1}" ]]; then
    mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT id_rundet FROM moller_run_details WHERE rundet_type LIKE 'beam_pol%' OR rundet_type='spin_dance' OR rundet_type = 'false_asym';" > BURSTLIST.txt    
else
    mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT id_rundet FROM moller_run_details WHERE FLOOR(rundet_pcrex_group)=${1};" > BURSTLIST.txt
fi

## START ON THE LIST OF RUNS
while IFS=: read -r RUNNUMB; do
    #GET THE ANPOWER FOR THE RUN... MAY DO POLARIZATIONS LATER ON INSTEAD OF ASYMMETRY.
    ANPOWER=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT rundet_anpow FROM moller_run_details WHERE id_rundet=${RUNNUMB};")

    PATFILE="${MROOTDR}/molana_patterns_${RUNNUMB}.root";

    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",0)";
    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",1)";
    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",2)";
    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",5)";
    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",10)";
    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",25)";
    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",50)";
    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",100)";
    root -b -l -q "molana_run_burst_analysis.C+(\""${PATFILE}\"","${TARGPOL}","${ANPOWER}",250)";

done < BURSTLIST.txt

root -b -l -q "gather_run_burst_analysis.C"

mv Burst_Comparison_Run_*.png ${RSLTDIR}/burst/
