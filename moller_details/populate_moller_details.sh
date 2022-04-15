#!/bin/bash

DETDIR=$1

for file in `ls ${DETDIR}/*.csv | sort -V`; do
  INPUTFILE=$file
  echo "${INPUTFILE}";
  while IFS=, read -r DATE BEGINRUN ENDINGRUN ANPOW TYPE GROUP COMMENT EXPERIMENT; do
    if [ -z "$DATE" ]; then
      DATE=NULL
    else 
      DATE=\"${DATE}\"
    fi

    if [ -z "$ANPOW" ]; then
      ANPOW=NULL
    fi

    if [ -z "$TYPE" ]; then
      TYPE=NULL
    else 
      TYPE=\"${TYPE}\"
    fi

    if [ -z "$GROUP" ]; then
      GROUP=NULL
    else 
      GROUP=\"${GROUP}\"
    fi

    if [ -z "$COMMENT" ]; then
      COMMENT=NULL
    else 
      COMMENT=\"${COMMENT}\"
    fi

    if [ -z "$EXPERIMENT" ]; then
      EXPERIMENT=NULL
    else 
      EXPERIMENT=\"${EXPERIMENT}\"
    fi

    for ((RUN=BEGINRUN;RUN<=ENDINGRUN;RUN++)); do

DB_COMMAND="UPDATE moller_run_details SET \
rundet_day = $DATE,\
rundet_anpow = $ANPOW,\
rundet_type = $TYPE,\
rundet_pcrex_group = $GROUP,\
rundet_comment = $COMMENT,\
experiment = $EXPERIMENT WHERE \
id_rundet = $RUN;"

    echo -e "${DB_COMMAND} \n";

    mysql -h halladb --user="hamolpol_admin" --password="5PHKFyrstvy3hDXp" --database="hamolpol" -e "${DB_COMMAND}"
    done
  done < $INPUTFILE
done
