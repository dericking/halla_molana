#!/bin/bash

TARGPOL=0.08005;

ANALRUN=$1;

##COPY LARGER ENV VAR NAMES TO COMPACT
MDBHOST=${MOLANA_DB_HOST}
MDBUSER=${MOLANA_DB_USER}
MDBPASS=${MOLANA_DB_PASS}
MDBNAME=${MOLANA_DB_NAME}
MROOTDR=${MOLLER_ROOTFILE_DIR}
ANALDIR=${MOLANA_DATADECODER_DIR}
RSLTDIR=${MOLANA_ONLINE_PUSH_DIR}

rm -f BURSTANALYSIS.txt

#[ -d "${RSLTDIR}/burst" ] && echo "run_molana_analysis() ==> Specified analysis files directory exists." || mkdir "${RSLTDIR}/burst"

## WE CAN SPECIFY A SINGLE GROUP OR LET IT AUTOMATICALLY POPULATE ALL GROUPS
if [[ -z "${1}" ]]; then
    mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT id_rundet FROM moller_run_details WHERE rundet_type LIKE 'beam_pol%' OR rundet_type='spin_dance' OR rundet_type = 'false_asym';" > BURSTANALYSIS.txt    
else
    echo "${ANALRUN}" > BURSTANALYSIS.txt
fi

## START ON THE LIST OF RUNS
while IFS=: read -r RUNNUMB; do
    #GET THE ANPOWER FOR THE RUN... MAY DO POLARIZATIONS LATER ON INSTEAD OF ASYMMETRY.

    if [[ ${RUNNUMB} < 19368 ]]; then 
      echo "skipping ${RUNNUMB}";
      continue; 
    fi

    PATFILE="molana_patterns_${RUNNUMB}.root";

    root -b -l -q "molana_pattern_analysis.C+(\""${PATFILE}"\")";


done < BURSTANALYSIS.txt
