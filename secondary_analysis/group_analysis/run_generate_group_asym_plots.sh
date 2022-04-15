#!/bin/bash

STARTGRP=$1;

##COPY LARGER ENV VAR NAMES TO COMPACT
MDBHOST=${MOLANA_DB_HOST}
MDBUSER=${MOLANA_DB_USER}
MDBPASS=${MOLANA_DB_PASS}
MDBNAME=${MOLANA_DB_NAME}
MROOTDR=${MOLLER_ROOTFILE_DIR}
ANALDIR=${MOLANA_DATADECODER_DIR}
RSLTDIR=${MOLANA_ONLINE_PUSH_DIR}

rm -f GROUPLIST2.txt

[ -d "${RSLTDIR}/group" ] && echo "run_molana_analysis() ==> Specified analysis files directory exists." || mkdir "${RSLTDIR}/group"

## WE CAN SPECIFY A SINGLE GROUP OR LET IT AUTOMATICALLY POPULATE ALL GROUPS
#if [[ -z "${1}" ]]; then
#    mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT DISTINCT FLOOR(rundet_pcrex_group) FROM moller_run_details WHERE rundet_type = 'beam_pol' OR rundet_type = 'false_asym' OR rundet_type = 'spin_dance' ORDER BY FLOOR(rundet_pcrex_group) ASC;" > GROUPLIST2.txt    
#else
    echo "${1}" >> GROUPLIST2.txt
#fi

## START ON THE LIST OF GROUPS
while IFS=: read -r GROUPNUM; do

    echo ${GROUPNUM};

    if(( GROUPNUM < 1001 )); then
            continue;
    fi

    ### GET THE FOUR FIT VALUES AND THEN PASS TO SCRIPT

    PATTPOL0VAL=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_asym FROM pcrex_groups WHERE id_group = ${GROUPNUM};")
    PATTPOL0ERR=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_asym_err FROM pcrex_groups WHERE id_group = ${GROUPNUM};")
    PATTGAUSVAL=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_asymgaus FROM pcrex_groups WHERE id_group = ${GROUPNUM};")
    PATTGAUSERR=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_asymgaus_err FROM pcrex_groups WHERE id_group = ${GROUPNUM};")
    PBLKPOL0VAL=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_block_asym FROM pcrex_groups WHERE id_group = ${GROUPNUM};")
    PBLKPOL0ERR=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_block_asym_err FROM pcrex_groups WHERE id_group = ${GROUPNUM};")
    PBLKGAUSVAL=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_block_asymgauss FROM pcrex_groups WHERE id_group = ${GROUPNUM};")
    PBLKGAUSERR=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT group_block_asymgauss_err FROM pcrex_groups WHERE id_group = ${GROUPNUM};")

    root -b -l -q "genasymplots.C+("${GROUPNUM}","${PATTPOL0VAL}","${PATTPOL0ERR}","${PATTGAUSVAL}","${PATTGAUSERR}","${PBLKPOL0VAL}","${PBLKPOL0ERR}","${PBLKGAUSVAL}","${PBLKGAUSERR}")"

done < GROUPLIST2.txt
