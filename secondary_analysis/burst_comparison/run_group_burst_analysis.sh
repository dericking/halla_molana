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

rm -f BURSTGROUPLIST.txt

[ -d "${RSLTDIR}/burst" ] && echo "run_molana_analysis() ==> Specified analysis files directory exists." || mkdir "${RSLTDIR}/burst"

## WE CAN SPECIFY A SINGLE GROUP OR LET IT AUTOMATICALLY POPULATE ALL GROUPS
if [[ -z "${1}" ]]; then
    mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT DISTINCT id_group FROM pcrex_groups WHERE group_type LIKE 'beam_pol%' OR group_type='spin_dance' OR group_type = 'false_asym' AND id_group > 1000 ORDER BY id_group DESC;" > BURSTGROUPLIST.txt    
else
    echo "${1}" >> BURSTGROUPLIST.txt
fi

## START ON THE LIST OF RUNS
while IFS=: read -r GRPNUMB; do

  ###REMOVE ME
  if [[ $GRPNUMB < 5000 ]]; then

    #GET THE ANPOWER FOR THE RUN... MAY DO POLARIZATIONS LATER ON INSTEAD OF ASYMMETRY.
    ANPOWER=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_anpow FROM pcrex_groups WHERE id_group=${GRPNUMB};")

    if [[ -z "$ANPOWER" || "$ANPOWER" == "NULL" ]]; then
               ANPOWER=0.777777;
               echo "GROUP ${GRPNUMB} HAS INCONSISTENT ANPOWERS." >> GROUPBURSTERRORS.TXT
    fi

    GRPRUNS=$( mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT id_rundet FROM moller_run_details WHERE rundet_pcrex_group = ${GRPNUMB} ORDER BY id_rundet ASC;" | paste -sd ,)

    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",0)";
    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",1)";
    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",2)";
    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",5)";
    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",10)";
    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",25)";
    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",50)";
    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",100)";
    root -b -l -q "molana_group_burst_analysis.C+(\""${GRPRUNS}\"","${GRPNUMB}","${TARGPOL}","${ANPOWER}",250)";

  ###
  fi


done < BURSTGROUPLIST.txt

root -b -l -q "gather_group_burst_analysis.C"

mv Burst_Comparison_Group_*.png ${RSLTDIR}/burst/
