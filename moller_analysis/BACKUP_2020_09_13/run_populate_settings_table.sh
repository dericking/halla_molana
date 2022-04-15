#!/bin/bash

STARTRUN=$1
if [[ -z ${2} ]]; then
ENDRUN=$1
else
ENDRUN=${2}
fi

echo "Populating or re-populating database table for moller settings "
echo "from run $STARTRUN to $ENDRUN."

for i in $(seq $STARTRUN $ENDRUN);do

    ./populate_settings_in_molpol_db.sh ${i};

done
