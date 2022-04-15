#!/bin/bash

##COPY LARGER ENV VAR NAMES TO COMPACT
MDBHOST=${MOLANA_DB_HOST}
MDBUSER=${MOLANA_DB_USER}
MDBPASS=${MOLANA_DB_PASS}
MDBNAME=${MOLANA_DB_NAME}

## WE CAN SPECIFY A SINGLE GROUP OR LET IT AUTOMATICALLY POPULATE ALL GROUPS
if [[ -z "${1}" ]]; then
    mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT id_run FROM moller_run ORDER BY id_run ASC;" > GET_MAGNETS_LIST.txt
else
    echo "${1}" >> GET_MAGNETS_LIST.txt
fi

## START ON THE LIST OF GROUPS
while IFS=: read -r RUNNUMB; do
    if(( RUNNUMB < 15000 )); then 
            continue; 
    fi

    echo " ";
    echo "Checking run number: ${RUNNUMB}";

    ##IF RUN DOESN'T EXIST IN MOLLER RUN DATABASE, QUIT
    RUNCHECK=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT id_run FROM moller_run WHERE id_run = ${RUNNUMB};")
    if [[ -z $RUNCHECK || "$RUNCHECK" ==  "NULL" ]]; then
        echo "   !!!!! Results for run number do not exist in 'moller_run' data table.";
        continue;
    fi
    echo "Entry in moller_run table for run number ${RUNCHECK} exists.";


    ##IF RUN DOESN'T EXIST IN MOLLER SETTINGS DATABASE, QUIT
    RUNCHECK=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT id_set_run FROM moller_settings WHERE id_set_run = ${RUNNUMB};")
    if [[ -z $RUNCHECK || "$RUNCHECK" ==  "NULL" ]]; then
        echo "   !!!!! Results for run number do not exist in 'moller_settings' data table.";
        continue;
    fi
    echo "Entry in moller_settings table for run number ${RUNCHECK} exists.";


    ##IF GROUP TYPE DOESN'T EXIST IN DATABASE CHECK AGAINST RUN DATABASE
    RUNCONF=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT id_runmag FROM moller_run_magnets WHERE id_runmag = ${RUNCHECK};")
    if [[ -z $RUNCONF || "$RUNCONF" ==  "NULL" ]]; then
        echo "Results for run number do not exist in 'moller_run_magnets' data table.";
        echo "   >>>>> Querying EPICS archive for the moller magnet set values.";
    else
        echo "Run already exists in moller_run_magnets table.";
        echo "   !!!!! Will not query the EPICS Archive again.";
        continue;
    fi

    RUNCONF=${RUNCHECK}

    BEGINTIME=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT set_run_start FROM moller_settings WHERE id_set_run = ${RUNCONF}");

    ENDTIME=$(mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "SELECT set_run_end FROM moller_settings WHERE id_set_run = ${RUNCONF}");

    echo "Run ${RUNCONF} started at ${BEGINTIME} and ended at ${ENDTIME}";

    #RUNMAGS=$(myData -b "${BEGINTIME}" -e "${ENDTIME}" MQO1H02.S MQM1H02.S MQO1H03.S MQO1H03A.S MMA1H01.S hamolpol:am430:target);
    myData -b "${BEGINTIME}" -e "${ENDTIME}" MQO1H02.S MQM1H02.S MQO1H03.S MQO1H03A.S MMA1H01.S hamolpol:am430:target >> mag_sets_returned.txt

    RETURNED=$(sed '2q;d' mag_sets_returned.txt);

    echo $RETURNED;

    while IFS=" " read -r WHATDAY WHATTIME Q1 Q2 Q3 Q4 DP HH
    do
        mysql -h ${MDBHOST} --user="${MDBUSER}" --password="${MDBPASS}" --database="${MDBNAME}" --skip-column-names -e "INSERT INTO moller_run_magnets (id_runmag, runmag_q1set, runmag_q2set, runmag_q3set, runmag_q4set, runmag_dpset, runmag_hhset) VALUES (${RUNCONF}, ${Q1}, ${Q2}, ${Q3}, ${Q4}, ${DP}, ${HH});"
    done < <(printf '%s\n' "$RETURNED")

    rm -f mag_sets_returned.txt;

sleep 1;

done < GET_MAGNETS_LIST.txt

rm GET_MAGNETS_LIST.txt;
