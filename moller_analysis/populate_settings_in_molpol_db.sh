#!/bin/bash

MROOTDR=${MOLLER_ROOTFILE_DIR}
SETFILE="${MROOTDR}/mollerrun_${1}.set"

echo "Looking for settings file ${SETFILE}..."

NUMBER='^[+-]?[0-9]+([.][0-9]+)?$'

ID_SET_RUN=$(sed -n -e "s#\(Run Number : \)##p" $SETFILE)
if [[ -z "$ID_SET_RUN" ]]; then VARIABLE=${1}; fi;
SET_RUN_TYPE=$(sed -n -e "s#\(Run Type   : \)##p" $SETFILE)
if [[ -z "$SET_RUN_TYPE" ]]; then SET_RUN_TYPE=null; fi;

SET_RUN_START=$(sed -n -e "s#\(Date       : \)##p" $SETFILE)
echo "SET_RUN_START: ${SET_RUN_START}"

#####################################################################
# SET_RUN_END=$(sed -n -e "s#\(End Run Time : \)##p" $SETFILE)
# New settings file has Ending Run Time twice sometimes, following prevents problem arising from this
SET_RUN_END=$(sed -n -e "s#\(End Run Time : \)##p" $SETFILE)

echo "End Time Read #1: ${SET_RUN_END}"

touch tempfile
echo ${SET_RUN_END} > tempfile;
sed -E -i "s/(.* [[:digit:]]{4}) .*/\1/" tempfile
SET_RUN_END=$( head -n 1 tempfile )
rm tempfile

echo "End Time Read #2: ${SET_RUN_END}"
#####################################################################

#if [[ $ID_SET_RUN -gt 20157  ]]
#then
  #CONVERT TO FORMAT THAT SQL LIKES
  read dayname datnum month clock zone year <<< ${SET_RUN_START}
  SET_RUN_START=$(echo "$dayname $month $datnum $clock $zone $year")
  echo "Corrected SET_RUN_START: ${SET_RUN_START}"
  SET_RUN_START=$(date --date="$(printf "${SET_RUN_START}")" +"%Y-%m-%d %H:%M:%S")
  if [[ -z "$SET_RUN_START" ]]; then SET_RUN_START=null; fi;
  read dayname datnum month clock zone year <<< ${SET_RUN_END}
  SET_RUN_END=$(echo "$dayname $month $datnum $clock $zone $year")
  echo "Corrected SET_RUN_END: ${SET_RUN_END}"
  SET_RUN_END=$(date --date="$(printf "${SET_RUN_END}")" +"%Y-%m-%d %H:%M:%S")
  if [[ -z "$SET_RUN_END" ]]; then SET_RUN_END=null; fi;
#else 
#  #CONVERT TO FORMAT THAT SQL LIKES
#  read dayname month datnum clock zone year <<< ${SET_RUN_START}
#  SET_RUN_START=$(date --date="$(printf "${SET_RUN_START}")" +"%Y-%m-%d %H:%M:%S")
#  if [[ -z "$SET_RUN_START" ]]; then SET_RUN_START=null; fi;
#  read dayname month datnum clock zone year <<< ${SET_RUN_END}
#  SET_RUN_END=$(date --date="$(printf "${SET_RUN_END}")" +"%Y-%m-%d %H:%M:%S")
#  if [[ -z "$SET_RUN_END" ]]; then SET_RUN_END=null; fi;
#fi

#WE CAN CALCULATE THIS ONE
START_TIMESTAMP=$(date --date="$SET_RUN_START" +"%s")
END_TIMESTAMP=$(date --date="$SET_RUN_END" +"%s")
TIMESTAMP_DIFF=$((END_TIMESTAMP-START_TIMESTAMP));
#SET_RUN_LENGTH=date --date=${TIMESTAMP_DIFF} -u +%H:%M:%S
SET_RUN_LENGTH=$(printf '%02d:%02d:%02d' $((${TIMESTAMP_DIFF}/3600)) $((${TIMESTAMP_DIFF}%3600/60)) $((${TIMESTAMP_DIFF}%60)) )
echo "Start Time: ${SET_RUN_START}";
echo "  End Time: ${SET_RUN_END}";
echo " Time Diff: ${TIMESTAMP_DIFF}";
echo "Run length: ${SET_RUN_LENGTH}";

#TODO: FIXME:
#SET_TRIG_THRESH_CH0=$(sed -n -e "s#\(REPLACEME\)##p" $SETFILE)
#SET_TRIG_THRESH_CH1=$(sed -n -e "s#\(REPLACEME\)##p" $SETFILE)
SET_TRIG_THRESH_CH0=0
SET_TRIG_THRESH_CH1=1

#Altering variable value set commands to sed with wildcards, see below example, all others will be modified. 11-15-2022 Eric
#SET_IHWP_IN=$(sed -n -e "s#\(Laser 1/2 wave plate                 IGL1I00OD16_16       : \)##p" $SETFILE)
SET_IHWP_IN=$(sed -n -e "s#\(.*IGL1I00OD16_16.*: \)##p" $SETFILE)
if [[ -z "$SET_IHWP_IN" ]]; then SET_IHWP_IN=null; fi;

SET_E_BEAM=$(sed -n -e "s#\(.*HALLA:p.*: \)##p" $SETFILE)
SET_E_BEAM=$(printf "%.14f" $SET_E_BEAM)
if [[ -z "$SET_E_BEAM" ]]; then SET_E_BEAM=null; fi;

SET_E_INJ=$(sed -n -e "s#\(.*MMSINJEGAIN.*: \)##p" $SETFILE)
if [[ -z "$SET_E_INJ" ]]; then SET_E_INJ=null; fi;

SET_E_SLINAC=$(sed -n -e "s#\(.*MMSLIN1EGAIN.*: \)##p" $SETFILE)
if [[ -z "$SET_E_SLINAC" ]]; then SET_E_SLINAC=null; fi;

SET_E_NLINAC=$(sed -n -e "s#\(.*MMSLIN2EGAIN.*: \)##p" $SETFILE)
if [[ -z "$SET_E_NLINAC" ]]; then SET_E_NLINAC=null; fi;

SET_N_PASS=$(sed -n -e "s#\(.*MMSHLAPASS.*: \)##p" $SETFILE)
if [[ -z "$SET_N_PASS" ]]; then SET_N_PASS=null; fi;

SET_BCM_AVG=$(sed -n -e "s#\(.*hac_bcm_average.*: \)##p" $SETFILE)
if [[ -z "$SET_BCM_AVG" ]]; then SET_BCM_AVG=null; fi;

SET_UNSER=$(sed -n -e "s#\(.*hac_unser_read.*: \)##p" $SETFILE)
if [[ -z "$SET_UNSER" ]]; then SET_UNSER=null; fi;

SET_BCM_US=$(sed -n -e "s#\(.*hac_bcm_dvm1_current.*: \)##p" $SETFILE)
if [[ -z "$SET_BCM_US" ]]; then SET_BCM_US=null; fi;

SET_BCM_DS=$(sed -n -e "s#\(.*hac_bcm_dvm2_current.*: \)##p" $SETFILE)
if [[ -z "$SET_BCM_DS" ]]; then SET_BCM_DS=null; fi;

##########################################################################################
# New settings file has IBC0L02Current twice, following prevents problem arising from this
SET_INJ_BCM_TOT=$(sed -n -e "s#\(.*IBC0L02Current.*: \)##p" $SETFILE)

touch tempfile
echo ${SET_INJ_BCM_TOT} > tempfile;
sed -i 's# .*##g' tempfile
SET_INJ_BCM_TOT=$( printf %.9f $(head -n 1 tempfile) )
rm tempfile

if [[ -z "$SET_INJ_BCM_TOT" ]]; then SET_INJ_BCM_TOT=null; fi;
##########################################################################################

##########################################################################################
# New settings file has IBC1H04CRCUR2 twice, following prevents problem arising from this
SET_INJ_BCM_HALLA=$(sed -n -e "s#\(.*IBC1H04CRCUR2.*: \)##p" $SETFILE)

touch tempfile
echo ${SET_INJ_BCM_HALLA} > tempfile;
sed -i 's# .*##g' tempfile
SET_INJ_BCM_HALLA=$( printf %.9f $(head -n 1 tempfile) )
rm tempfile

if [[ -z "$SET_INJ_BCM_HALLA" ]]; then SET_INJ_BCM_HALLA=null; fi;
##########################################################################################

SET_BPM01_X=$(sed -n -e "s#\(.*IPM1H01.XPOS.*: \)##p" $SETFILE)
if [[ -z "$SET_BPM01_X" ]]; then SET_BPM01_X=null; fi;

SET_BPM01_Y=$(sed -n -e "s#\(.*IPM1H01.YPOS.*: \)##p" $SETFILE)
if [[ -z "$SET_BPM01_Y" ]]; then SET_BPM01_Y=null; fi;

SET_BPM04_X=$(sed -n -e "s#\(.*IPM1H04.XPOS.*: \)##p" $SETFILE)
if [[ -z "$SET_BPM04_X" ]]; then SET_BPM04_X=null; fi;

SET_BPM04_Y=$(sed -n -e "s#\(.*IPM1H04.YPOS.*: \)##p" $SETFILE)
if [[ -z "$SET_BPM04_Y" ]]; then SET_BPM04_Y=null; fi;

SET_BPM04A_X=$(sed -n -e "s#\(.*IPM1H04A.XPOS.*: \)##p" $SETFILE)
if [[ -z "$SET_BPM04A_X" ]]; then SET_BPM04A_X=null; fi;

SET_BPM04A_Y=$(sed -n -e "s#\(.*IPM1H04A.YPOS.*: \)##p" $SETFILE)
if [[ -z "$SET_BPM04A_Y" ]]; then SET_BPM04A_Y=null; fi;

SET_Q1_CUR=$(sed -n -e "s#\(.*MQO1H02M.*: \)##p" $SETFILE)
if [[ -z "$SET_Q1_CUR" ]]; then SET_Q1_CUR=null; fi;

SET_Q2_CUR=$(sed -n -e "s#\(.*MQM1H02M.*: \)##p" $SETFILE)
if [[ -z "$SET_Q2_CUR" ]]; then SET_Q2_CUR=null; fi;

SET_Q3_CUR=$(sed -n -e "s#\(.*MQO1H03M.*: \)##p" $SETFILE)
if [[ -z "$SET_Q3_CUR" ]]; then SET_Q3_CUR=null; fi;

SET_Q4_CUR=$(sed -n -e "s#\(.*MQO1H03AM.*: \)##p" $SETFILE)
if [[ -z "$SET_Q4_CUR" ]]; then SET_Q4_CUR=null; fi;

SET_DIP_CUR=$(sed -n -e "s#\(.*MMA1H01M.*: \)##p" $SETFILE)
if [[ -z "$SET_DIP_CUR" ]]; then SET_DIP_CUR=null; fi;

SET_TGT_ANGLE=$(sed -n -e "s#\(.*HAHFMROTENC.*: \)##p" $SETFILE)
if [[ -z "$SET_TGT_ANGLE" ]]; then SET_TGT_ANGLE=null; fi;
if ! [[ $SET_TGT_ANGLE =~ $NUMBER ]]; then SET_TGT_ANGLE=null; fi;

SET_TGT_ANGLE_DEG=$(sed -n -e "s#\(.*HAHFMROTSM.RBV.*: \)##p" $SETFILE)
if [[ -z "$SET_TGT_ANGLE_DEG" ]]; then SET_TGT_ANGLE_DEG=null; fi;
if ! [[ $SET_TGT_ANGLE_DEG =~ $NUMBER ]]; then SET_TGT_ANGLE_DEG=null; fi;

SET_TGT_LIN_POS=$(sed -n -e "s#\(.*HAHFMLINENC.*: \)##p" $SETFILE)
if [[ -z "$SET_TGT_LIN_POS" ]]; then SET_TGT_LIN_POS=null; fi;
if ! [[ $SET_TGT_LIN_POS =~ $NUMBER ]]; then SET_TGT_LIN_POS=null; fi;

SET_TARGET=null;
SET_TGT_LIN_POS_MM=$(sed -n -e "s#\(.*HAHFMLINSM.RBV.*: \)##p" $SETFILE)
if [[ -z "$SET_TGT_LIN_POS_MM" ]]; then SET_TGT_LIN_POS_MM=null; 
elif ! [[ $SET_TGT_LIN_POS_MM =~ $NUMBER ]]; then SET_TGT_LIN_POS_MM=null;
else
    SET_TARGET=-1   #already set to null, so if position is listed we can calculate which target
    step=35.0       #distance between targets
    tgt1_pos=61.0   #position in mm of first target
    tgt_pos=$SET_TGT_LIN_POS_MM
    tol=3.0         #allowed variation
    if [ $(echo "$tgt_pos>$tgt1_pos-$tol" | bc) -eq "1" ] && [ $(echo "$tgt_pos<$tgt1_pos+$tol" | bc) -eq "1" ]; then
        SET_TARGET=1;
    elif  [ $(echo "$tgt_pos>$tgt1_pos+$step-$tol" | bc) -eq "1" ]   &&  [ $(echo "$tgt_pos<$tgt1_pos+$step+$tol" | bc) -eq "1" ]; then
        SET_TARGET=2;
    elif  [ $(echo "$tgt_pos>$tgt1_pos+2*$step-$tol" | bc) -eq "1" ] &&  [ $(echo "$tgt_pos<$tgt1_pos+2*$step+$tol" | bc) -eq "1" ]; then
        SET_TARGET=3;
    elif  [ $(echo "$tgt_pos>$tgt1_pos+3*$step-$tol" | bc) -eq "1" ] &&  [ $(echo "$tgt_pos<$tgt1_pos+3*$step+$tol" | bc) -eq "1" ]; then
        SET_TARGET=4;
    else
        SET_TARGET=0;
    fi;
    echo "Target number set to: ${SET_TARGET}"
fi;

SET_LAS_MODE_HALLA=$(sed -n -e "s#\(.*IGL1I00HALLAMODE.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_MODE_HALLA" ]]; then SET_LAS_MODE_HALLA=null; fi;

SET_LAS_MODE_HALLB=$(sed -n -e "s#\(.*IGL1I00HALLBMODE.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_MODE_HALLB" ]]; then SET_LAS_MODE_HALLB=null; fi;

SET_LAS_MODE_HALLC=$(sed -n -e "s#\(.*IGL1I00HALLCMODE.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_MODE_HALLC" ]]; then SET_LAS_MODE_HALLC=null; fi;

SET_LAS_POW_HALLA=$(sed -n -e "s#\(.*IGL1I00DAC0.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_POW_HALLA" ]]; then SET_LAS_POW_HALLA=null; fi;

SET_LAS_POW_HALLB=$(sed -n -e "s#\(.*IGL1I00DAC2.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_POW_HALLB" ]]; then SET_LAS_POW_HALLB=null; fi;

SET_LAS_POW_HALLC=$(sed -n -e "s#\(.*IGL1I00DAC4.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_POW_HALLC" ]]; then SET_LAS_POW_HALLC=null; fi;

SET_LAS_ATTN_HALLA=$(sed -n -e "s#\(.*psub_aa_pos.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_ATTN_HALLA" ]]; then SET_LAS_ATTN_HALLA=null; fi;

SET_LAS_ATTN_HALLB=$(sed -n -e "s#\(.*psub_ab_pos.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_ATTN_HALLB" ]]; then SET_LAS_ATTN_HALLB=null; fi;

SET_LAS_ATTN_HALLC=$(sed -n -e "s#\(.*psub_ac_pos.*: \)##p" $SETFILE)
if [[ -z "$SET_LAS_ATTN_HALLC" ]]; then SET_LAS_ATTN_HALLC=null; fi;

SET_SLIT_HALLA=$(sed -n -e "s#\(.*SMRPOSA.*: \)##p" $SETFILE)
if [[ -z "$SET_SLIT_HALLA" ]]; then SET_SLIT_HALLA=null; fi;

SET_SLIT_HALLB=$(sed -n -e "s#\(.*SMRPOSB.*: \)##p" $SETFILE)
if [[ -z "$SET_SLIT_HALLB" ]]; then SET_SLIT_HALLB=null; fi;

SET_SLIT_HALLC=$(sed -n -e "s#\(.*SMRPOSC.*: \)##p" $SETFILE)
if [[ -z "$SET_SLIT_HALLC" ]]; then SET_SLIT_HALLC=null; fi;

SET_IHWP=$(sed -n -e "s#\(.*IGL1I00OD16_16.*: \)##p" $SETFILE)
if [[ -z "$SET_IHWP" ]]; then SET_IHWP=null; fi;

SET_RHWP=$(sed -n -e "s#\(.*psub_pl_pos.*: \)##p" $SETFILE)
if [[ -z "$SET_RHWP" ]]; then SET_RHWP=null; fi;

SET_VWIEN_ANGLE=$(sed -n -e "s#\(.*VWienAngle.*: \)##p" $SETFILE)
if [[ -z "$SET_VWIEN_ANGLE" ]]; then SET_VWIEN_ANGLE=null; fi;

SET_SOL_PHI_FG=$(sed -n -e "s#\(.*Phi_FG.*: \)##p" $SETFILE)
if [[ -z "$SET_SOL_PHI_FG" ]]; then SET_SOL_PHI_FG=null; fi;

SET_HWIEN_ANGLE=$(sed -n -e "s#\(.*HWienAngle.*: \)##p" $SETFILE)
if [[ -z "$SET_HWIEN_ANGLE" ]]; then SET_HWIEN_ANGLE=null; fi;

SET_HEL_PATTERN=$(sed -n -e "s#\(.*HELPATTERNd.*: \)##p" $SETFILE)
if [[ -z "$SET_HEL_PATTERN" ]]; then SET_HEL_PATTERN=null; fi;

SET_HEL_FREQ=$(sed -n -e "s#\(.*HELFREQ.*: \)##p" $SETFILE)
if [[ -z "$SET_HEL_FREQ" ]]; then SET_HEL_FREQ=null; fi;

SET_HEL_DELAY=$(sed -n -e "s#\(.*HELDELAYd.*: \)##p" $SETFILE)
if [[ -z "$SET_HEL_DELAY" ]]; then SET_HEL_DELAY=null; fi;

SET_T_SETTLE=$(sed -n -e "s#\(.*HELTSETTLEd.*: \)##p" $SETFILE)
if [[ -z "$SET_T_SETTLE" ]]; then SET_T_SETTLE=null; fi;

SET_T_STABLE=$(sed -n -e "s#\(.*HELTSTABLEd.*: \)##p" $SETFILE)
if [[ -z "$SET_T_STABLE" ]]; then SET_T_STABLE=null; fi;

SET_BPM02A_X=$(sed -n -e "s#\(.*IPM1P02A.XPOS.*: \)##p" $SETFILE)
if [[ -z "$SET_BPM02A_X" ]]; then SET_BPM02A_X=null; fi;

SET_BPM02A_Y=$(sed -n -e "s#\(.*IPM1P02A.YPOS.*: \)##p" $SETFILE)
if [[ -z "$SET_BPM02A_Y" ]]; then SET_BPM02A_Y=null; fi;

SET_MOL_MAG_CUR_SET=$(sed -n -e "s#\(.*hamolpol:am430:target .*: \)##p" $SETFILE)
echo "SET_MOL_MAG_CUR_SET: $SET_MOL_MAG_CUR_SET"
if [[ -z "$SET_MOL_MAG_CUR_SET" ]]; then SET_MOL_MAG_CUR_SET=null; fi;
if ! [[ $SET_MOL_MAG_CUR_SET =~ $NUMBER ]]; then SET_MOL_MAG_CUR_SET=null; fi;

#########################################################################################
#Next line is the updated sed statement, the following if[[]] is to reconcile a backwards
#compatibility issue with a changed and truncated EPICS variable name --Eric 11-15-22
SET_MOL_MAG_CUR_MEAS=$(sed -n -e "s#\(   AM430 Measured Current (A)        hamolpol:am430:targetRbck.*: \)##p" $SETFILE)
if [[ -z "$SET_MOL_MAG_CUR_MEAS" ]]; then SET_MOL_MAG_CUR_MEAS=$(sed -n -e "s#\(.*hamolpol:am430:magCu *: \)##p" $SETFILE); fi;
if [[ -z "$SET_MOL_MAG_CUR_MEAS" ]]; then SET_MOL_MAG_CUR_MEAS=null; fi;
if ! [[ $SET_MOL_MAG_CUR_MEAS =~ $NUMBER ]]; then SET_MOL_MAG_CUR_MEAS=null; fi;
#########################################################################################

#########################################################################################
#Next line is the updated sed statement, the following if[[]] is to reconcile a backwards
#compatibility issue with a truncated EPICS variable name --Eric 11-15-22
SET_MOL_MAG_V_MEAS=$(sed -n -e "s#\(.*hamolpol:am430:magVoltage.*: \)##p" $SETFILE)
if [[ -z "$SET_MOL_MAG_V_MEAS" ]]; then SET_MOL_MAG_V_MEAS=$(sed -n -e "s#\(.*hamolpol:am430:magVo.*: \)##p" $SETFILE); fi;
if [[ -z "$SET_MOL_MAG_V_MEAS" ]]; then SET_MOL_MAG_V_MEAS=null; fi;
if ! [[ $SET_MOL_MAG_V_MEAS =~ $NUMBER ]]; then SET_MOL_MAG_V_MEAS=null; fi;
#########################################################################################

#########################################################################################
#Next line is the updated sed statement, the following if[[]] is to reconcile a backwards
#compatibility issue with a truncated EPICS variable name --Eric 11-15-22
SET_MOL_MAG_FIELD_MEAS=$(sed -n -e "s#\(.*hamolpol:am430:magField.*: \)##p" $SETFILE)
if [[ -z "$SET_MOL_MAG_FIELD_MEAS" ]]; then SET_MOL_MAG_FIELD_MEAS=$(sed -n -e "s#\(.*hamolpol:am430:magFi.*: \)##p" $SETFILE); fi;
if [[ -z "$SET_MOL_MAG_FIELD_MEAS" ]]; then SET_MOL_MAG_FIELD_MEAS=null; fi;
if ! [[ $SET_MOL_MAG_FIELD_MEAS =~ $NUMBER ]]; then SET_MOL_MAG_FIELD_MEAS=null; fi;
#########################################################################################

#########################################################################################
#Next line is the updated sed statement, the following if[[]] is to reconcile a backwards
#compatibility issue with a truncated EPICS variable name --Eric 11-15-22
SET_MOL_MAG_RAMP_STATE=$(sed -n -e "s#\(.*hamolpol:am430:rampState.*: \)##p" $SETFILE)
if [[ -z "$SET_MOL_MAG_RAMP_STATE" ]]; then SET_MOL_MAG_RAMP_STATE=$(sed -n -e "s#\(   AMS430 Ramp State                 hamolpol:am430:rampS.*: \)##p" $SETFILE); fi;
if [[ -z "$SET_MOL_MAG_RAMP_STATE" ]]; then SET_MOL_MAG_RAMP_STATE=null; fi;
if ! [[ $SET_MOL_MAG_RAMP_STATE =~ $NUMBER ]]; then SET_MOL_MAG_RAMP_STATE=null; fi;
#########################################################################################

#########################################################################################
#Next line is the updated sed statement, the following if[[]] is to reconcile a backwards
#compatibility issue with a truncated EPICS variable name --Eric 11-15-22
SET_MOL_COOLER_TEMP=$(sed -n -e "s#\(.*hamolpol:lk218_1:temp1.*: \)##p" $SETFILE)
if [[ -z "$SET_MOL_COOLER_TEMP" ]]; then SET_MOL_COOLER_TEMP=$(sed -n -e "s#\(   Cryocooler Temperature (K)        hamolpol:lk218_1:tem.*: \)##p" $SETFILE); fi;
if [[ -z "$SET_MOL_COOLER_TEMP" ]]; then SET_MOL_COOLER_TEMP=null; fi;
if ! [[ $SET_MOL_COOLER_TEMP =~ $NUMBER ]]; then SET_MOL_COOLER_TEMP=null; fi;
#########################################################################################

#########################################################################################
#Next line is the updated sed statement, the following if[[]] is to reconcile a backwards
#compatibility issue with a truncated EPICS variable name --Eric 11-15-22
SET_MOL_MAG_T2TEMP=$(sed -n -e "s#\(.*hamolpol:lk218_1:temp2.*: \)##p" $SETFILE)
if [[ -z "$SET_MOL_MAG_T2TEMP" ]]; then SET_MOL_MAG_T2TEMP=$(sed -n -e "s#\(   Magnet(T2) Temperature(K)         hamolpol:lk218_1:tem.*: \)##p" $SETFILE); fi;
if [[ -z "$SET_MOL_MAG_T2TEMP" ]]; then SET_MOL_MAG_T2TEMP=null; fi;
if ! [[ $SET_MOL_MAG_T2TEMP =~ $NUMBER ]]; then SET_MOL_MAG_T2TEMP=null; fi;
#########################################################################################

#########################################################################################
#Next line is the updated sed statement, the following if[[]] is to reconcile a backwards
#compatibility issue with a truncated EPICS variable name --Eric 11-15-22
SET_MOL_MAG_LEAD1_TEMP=$(sed -n -e "s#\(.*hamolpol:lk218_1:temp6.*: \)##p" $SETFILE)
if [[ -z "$SET_MOL_MAG_LEAD1_TEMP" ]]; then SET_MOL_MAG_LEAD1_TEMP=$(sed -n -e "s#\(   Magnet Lead \#1 (T6) Temperature(K hamolpol:lk218_1:tem.*: \)##p" $SETFILE); fi;
if [[ -z "$SET_MOL_MAG_LEAD1_TEMP" ]]; then SET_MOL_MAG_LEAD1_TEMP=null; fi;
if ! [[ $SET_MOL_MAG_LEAD1_TEMP =~ $NUMBER ]]; then SET_MOL_MAG_LEAD1_TEMP=null; fi;
#########################################################################################

#########################################################################################
#Next line is the updated sed statement, the following if[[]] is to reconcile a backwards
#compatibility issue with a truncated EPICS variable name --Eric 11-15-22
SET_MOL_MAG_LEAD2_TEMP=$(sed -n -e "s#\(.*hamolpol:lk218_1:temp7.*: \)##p" $SETFILE)
if [[ -z "$SET_MOL_MAG_LEAD2_TEMP" ]]; then SET_MOL_MAG_LEAD2_TEMP=$(sed -n -e "s#\(   Magnet Lead \#2 (T7) Temperature(K hamolpol:lk218_1:tem.*: \)##p" $SETFILE); fi;
if [[ -z "$SET_MOL_MAG_LEAD2_TEMP" ]]; then SET_MOL_MAG_LEAD2_TEMP=null; fi;
if ! [[ $SET_MOL_MAG_LEAD2_TEMP =~ $NUMBER ]]; then SET_MOL_MAG_LEAD2_TEMP=null; fi;
#########################################################################################

SET_DET_HV_CH1=$(sed -n -e "s#\(Moller Detector measured HV ch 1     hatsv5.2003.S5.0.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_HV_CH1" ]]; then SET_DET_HV_CH1=null; fi;
SET_DET_HV_CH2=$(sed -n -e "s#\(Moller Detector measured HV ch 2     hatsv5.2003.S5.1.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_HV_CH2" ]]; then SET_DET_HV_CH2=null; fi;
SET_DET_HV_CH3=$(sed -n -e "s#\(Moller Detector measured HV ch 3     hatsv5.2003.S5.2.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_HV_CH3" ]]; then SET_DET_HV_CH3=null; fi;
SET_DET_HV_CH4=$(sed -n -e "s#\(Moller Detector measured HV ch 4     hatsv5.2003.S5.3.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_HV_CH4" ]]; then SET_DET_HV_CH4=null; fi;
SET_DET_HV_CH5=$(sed -n -e "s#\(Moller Detector measured HV ch 5     hatsv5.2003.S5.4.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_HV_CH5" ]]; then SET_DET_HV_CH5=null; fi;
SET_DET_HV_CH6=$(sed -n -e "s#\(Moller Detector measured HV ch 6     hatsv5.2003.S5.5.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_HV_CH6" ]]; then SET_DET_HV_CH6=null; fi;
SET_DET_HV_CH7=$(sed -n -e "s#\(Moller Detector measured HV ch 7     hatsv5.2003.S5.6.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_HV_CH7" ]]; then SET_DET_HV_CH7=null; fi;
SET_DET_HV_CH8=$(sed -n -e "s#\(Moller Detector measured HV ch 8     hatsv5.2003.S5.7.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_HV_CH8" ]]; then SET_DET_HV_CH8=null; fi;
SET_DET_AP_CH1=$(sed -n -e "s#\(Moller Detector measured HV Ap 1     hatsv5.2003.S6.0.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_AP_CH1" ]]; then SET_DET_AP_CH1=null; fi;
SET_DET_AP_CH2=$(sed -n -e "s#\(Moller Detector measured HV Ap 2     hatsv5.2003.S6.1.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_AP_CH2" ]]; then SET_DET_AP_CH2=null; fi;
SET_DET_AP_CH3=$(sed -n -e "s#\(Moller Detector measured HV Ap 3     hatsv5.2003.S6.2.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_AP_CH3" ]]; then SET_DET_AP_CH3=null; fi;
SET_DET_AP_CH4=$(sed -n -e "s#\(Moller Detector measured HV Ap 4     hatsv5.2003.S6.3.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_AP_CH4" ]]; then SET_DET_AP_CH4=null; fi;
SET_DET_AP_CH5=$(sed -n -e "s#\(Moller Detector measured HV Ap 5     hatsv5.2003.S6.4.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_AP_CH5" ]]; then SET_DET_AP_CH5=null; fi;
SET_DET_AP_CH6=$(sed -n -e "s#\(Moller Detector measured HV Ap 6     hatsv5.2003.S6.5.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_AP_CH6" ]]; then SET_DET_AP_CH6=null; fi;
SET_DET_AP_CH7=$(sed -n -e "s#\(Moller Detector measured HV Ap 7     hatsv5.2003.S6.6.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_AP_CH7" ]]; then SET_DET_AP_CH7=null; fi;
SET_DET_AP_CH8=$(sed -n -e "s#\(Moller Detector measured HV Ap 8     hatsv5.2003.S6.7.*: \)##p" $SETFILE)
if [[ -z "$SET_DET_AP_CH8" ]]; then SET_DET_AP_CH8=null; fi;

#TODO: ????
#SET_TRIG_THRESH=$(sed -n -e "s#\(REPLACEME\)##p" $SETFILE)
#SET_TRIG_TYPE=$(sed -n -e "s#\(REPLACEME\)##p" $SETFILE)
SET_TRIG_THRESH=0
SET_TRIG_TYPE=0000

SET_MOL_POW_SUP_CUR=$(sed -n -e "s#\(Hcoils current (Amps).* : \)##p" $SETFILE)



DB_COMMAND="REPLACE INTO moller_settings \
             (id_set_run,\
              set_run_type,\
              set_run_start,\
              set_run_end,\
              set_run_length,\
              set_trig_thresh_ch0,\
              set_trig_thresh_ch1,\
              set_ihwp_in,\
              set_target,\
              set_E_beam,\
              set_E_inj,\
              set_E_Slinac,\
              set_E_Nlinac,\
              set_n_pass,\
              set_bcm_avg,\
              set_unser,\
              set_bcm_us,\
              set_bcm_ds,\
              set_inj_bcm_tot,\
              set_inj_bcm_halla,\
              set_bpm01_X,\
              set_bpm01_Y,\
              set_bpm04_X,\
              set_bpm04_Y,\
              set_bpm04a_X,\
              set_bpm04a_Y,\
              set_q1_cur,\
              set_q2_cur,\
              set_q3_cur,\
              set_q4_cur,\
              set_dip_cur,\
              set_tgt_angle,\
              set_tgt_angle_deg,\
              set_tgt_lin_pos,\
              set_tgt_lin_pos_mm,\
              set_las_mode_halla,\
              set_las_mode_hallb,\
              set_las_mode_hallc,\
              set_las_pow_halla,\
              set_las_pow_hallb,\
              set_las_pow_hallc,\
              set_las_attn_halla,\
              set_las_attn_hallb,\
              set_las_attn_hallc,\
              set_slit_halla,\
              set_slit_hallb,\
              set_slit_hallc,\
              set_ihwp,\
              set_rhwp,\
              set_vwien_angle,\
              set_sol_phi_fg,\
              set_hwien_angle,\
              set_hel_pattern,\
              set_hel_freq,\
              set_hel_delay,\
              set_t_settle,\
              set_t_stable,\
              set_bpm02a_X,\
              set_bpm02a_Y,\
              set_mol_mag_cur_set,\
              set_mol_mag_cur_meas,\
              set_mol_mag_v_meas,\
              set_mol_mag_field_meas,\
              set_mol_mag_ramp_state,\
              set_mol_cooler_temp,\
              set_mol_mag_T2temp,\
              set_mol_mag_lead1_temp,\
              set_mol_mag_lead2_temp,\
              set_det_hv_ch1,\
              set_det_hv_ch2,\
              set_det_hv_ch3,\
              set_det_hv_ch4,\
              set_det_hv_ch5,\
              set_det_hv_ch6,\
              set_det_hv_ch7,\
              set_det_hv_ch8,\
              set_det_ap_ch1,\
              set_det_ap_ch2,\
              set_det_ap_ch3,\
              set_det_ap_ch4,\
              set_det_ap_ch5,\
              set_det_ap_ch6,\
              set_det_ap_ch7,\
              set_det_ap_ch8,\
              set_trig_thresh,\
              set_trig_type,\
              set_mol_pow_sup_cur) \
              VALUES \
             (${ID_SET_RUN},\
             \"$SET_RUN_TYPE\",\
             \"$SET_RUN_START\",\
             \"$SET_RUN_END\",\
             \"$SET_RUN_LENGTH\",\
             $SET_TRIG_THRESH_CH0,\
             $SET_TRIG_THRESH_CH1,\
             \"$SET_IHWP_IN\",\
             $SET_TARGET,\
             $SET_E_BEAM,\
             $SET_E_INJ,\
             $SET_E_SLINAC,\
             $SET_E_NLINAC,\
             \"$SET_N_PASS\",\
             $SET_BCM_AVG,\
             $SET_UNSER,\
             $SET_BCM_US,\
             $SET_BCM_DS,\
             $SET_INJ_BCM_TOT,\
             $SET_INJ_BCM_HALLA,\
             $SET_BPM01_X,\
             $SET_BPM01_Y,\
             $SET_BPM04_X,\
             $SET_BPM04_Y,\
             $SET_BPM04A_X,\
             $SET_BPM04A_Y,\
             $SET_Q1_CUR,\
             $SET_Q2_CUR,\
             $SET_Q3_CUR,\
             $SET_Q4_CUR,\
             $SET_DIP_CUR,\
             $SET_TGT_ANGLE,\
             $SET_TGT_ANGLE_DEG,\
             $SET_TGT_LIN_POS,\
             $SET_TGT_LIN_POS_MM,\
             \"$SET_LAS_MODE_HALLA\",\
             \"$SET_LAS_MODE_HALLB\",\
             \"$SET_LAS_MODE_HALLC\",\
             $SET_LAS_POW_HALLA,\
             $SET_LAS_POW_HALLB,\
             $SET_LAS_POW_HALLC,\
             $SET_LAS_ATTN_HALLA,\
             $SET_LAS_ATTN_HALLB,\
             $SET_LAS_ATTN_HALLC,\
             $SET_SLIT_HALLA,\
             $SET_SLIT_HALLB,\
             $SET_SLIT_HALLC,\
             \"$SET_IHWP\",\
             $SET_RHWP,\
             $SET_VWIEN_ANGLE,\
             $SET_SOL_PHI_FG,\
             $SET_HWIEN_ANGLE,\
             \"$SET_HEL_PATTERN\",\
             $SET_HEL_FREQ,\
             \"$SET_HEL_DELAY\",\
             $SET_T_SETTLE,\
             $SET_T_STABLE,\
             $SET_BPM02A_X,\
             $SET_BPM02A_Y,\
             $SET_MOL_MAG_CUR_SET,\
             $SET_MOL_MAG_CUR_MEAS,\
             $SET_MOL_MAG_V_MEAS,\
             $SET_MOL_MAG_FIELD_MEAS,\
             $SET_MOL_MAG_RAMP_STATE,\
             $SET_MOL_COOLER_TEMP,\
             $SET_MOL_MAG_T2TEMP,\
             $SET_MOL_MAG_LEAD1_TEMP,\
             $SET_MOL_MAG_LEAD2_TEMP,\
             $SET_DET_HV_CH1,\
             $SET_DET_HV_CH2,\
             $SET_DET_HV_CH3,\
             $SET_DET_HV_CH4,\
             $SET_DET_HV_CH5,\
             $SET_DET_HV_CH6,\
             $SET_DET_HV_CH7,\
             $SET_DET_HV_CH8,\
             $SET_DET_AP_CH1,\
             $SET_DET_AP_CH2,\
             $SET_DET_AP_CH3,\
             $SET_DET_AP_CH4,\
             $SET_DET_AP_CH5,\
             $SET_DET_AP_CH6,\
             $SET_DET_AP_CH7,\
             $SET_DET_AP_CH8,\
             $SET_TRIG_THRESH,\
             \"$SET_TRIG_TYPE\",\
             $SET_MOL_POW_SUP_CUR)"

echo ${DB_COMMAND}

mysql -h ${MOLANA_DB_HOST} --user="${MOLANA_DB_USER}" --password="${MOLANA_DB_PASS}" --database="${MOLANA_DB_NAME}" -e "${DB_COMMAND}";
