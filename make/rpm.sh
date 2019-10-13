#//////////////////////////////////////////////////////////////////////////////
#/                                                                           //
#/ Copyright (c) 2018 Ethinza Inc., All rights reserved                      //
#/                                                                           //
#/ This file contains TRADE SECRETs of ETHINZA INC. and is CONFIDENTIAL.     //
#/ Without written consent from ETHINZA INC., it is prohibited to disclose   //
#/ or reproduce its contents, or to manufacture, use or sell it in whole or  //
#/ part. Any reproduction of this file without written consent of ETHINZA    //
#/ INC. is a violation of the copyright laws and is subject to civil         //
#/ liability and criminal prosecution.                                       //
#/                                                                           //
#//////////////////////////////////////////////////////////////////////////////

# RPM wrapper
#

echo " "  
echo "Generate RPM files!"
echo " "  

set -x
#WHICH="uswitch ucli"
WHICH=$1
echo ${WHICH}
if [ -z "${WHICH}" ]; then
	echo "Please Supply File For Which RPM Will Be Built"
	read MPR
	WHICH=${MPR}
fi

if [ -z "${MTYPE}" ]; then
	MTYPE=`uname -m`
fi

sleep 2

#

# Set Up Fake Release String, If None Exported
if [ -z "${YDT}" ]; then
	YDT=`date +%y.%m.%d.%H`
fi

YADAT=`date +%D" "%T`

if [ -z "${ETHINZA_DIR}" ]; then
	MDIR=`pwd`
	export ETHINZA_DIR=${MDIR}/..
else
	MDIR=${ETHINZA_DIR}/rpm
fi


# Grab Some Build Info Since We Use Local Defaults:
KRNREL=`uname -r`
MACHON=`uname -n`

# Determine Build Level Below
# VERS Should Be Passed From Top Level Makefile

# Hard Code Major Minor Version Level - Need to Update
VRZN=1.0.0.0

# VRFL=include/version.h (or some such entity)
# Will There Be A Version File? Right Now Use Major-Date
#REL=`gawk '/VERSION/ {printf $3"."}' ${VRFL}`
REL=${YDT}

 
for i in ${WHICH}
do

 # Copy in Binary Or Tar For Packaging
 case ${i} in 
	ucli) BLD=${MDIR}/../apps/ucli/ucli ;
		RPMOPT=bb ;
		;;
	uswitch) BLD=${MDIR}/../apps/uswitch/uswitch ;
		RPMOPT=bb ;
		;;
	dma) RID=${MDIR}/../mods/dma ;
		BLD=${RID}.tar.gz ;
		cd ${RID}; tar -cf ${BLD} . ;
		RPMOPT=bs ;
		;;
	*)	echo "Don't Know Application" ;;
 esac

 # Determine Library Requirements Of Binary 
 if [ "${RPMOPT}" = "bb" ]; then
	SBIL=`ldd ${BLD} | gawk '/lib/ {printf $3" "}'`
	LIBS="${SBIL}"
 fi
 
 cd ${MDIR}
 mkdir BUILD RPMS SRPMS
 # Determine The Changes Since Last Build (For ChangeLog In Spec)
 #SRCLOG=`git log --oneline ${LASTMOD}..`

 SPC1=${i}.spec

 # Determine Size Of Binary Or Tar In Case Needed
 SIZ=`du -b ${BLD}  | cut -f1`
 echo ${SIZ}

 # Copy In Clean Header Files
 cp ${MDIR}/SPECS/${SPC1} .
 cp ${MDIR}/${SPC1} ${SPC1}b
 
 # Apply The Appropriate Release, Requirements, Machine Type, Date Version To Spec File
 sed -e "s>VVV>${VRZN}>" -e "s>RRR>${REL}>" -e "s>MMM>${MTYPE}>" -e "s>LLL>${LIBS}>" < ${SPC1}b > ${SPC1}
 
 # Clean Up Temp Spec File
 rm ${SPC1}b

 # Check The Spec File
 cat ${SPC1}

 # Set Up Level To Be Used In RPM AND TAG
 VRS=${VRZN}-${REL}

 # Set Up Rpm And Ftp Directory Names
 RPMFL=${i}-${VRS}.${MTYPE}.rpm

 FTPDIR=/var/ftp/ethinza/${VRS}
 
 if [ ! -d ${FTPDIR} ]; then
 	mkdir -p ${FTPDIR}
 fi
 INFOout=${FTPDIR}/BuildInfo

 # Build Rpm (Binary Or Source) Using Spec File
 rpmbuild -${RPMOPT} ${SPC1}
 mv ${SPC1} ${FTPDIR}
 cp -p ${MDIR}/RPMS/${MTYPE}/${RPMFL} ${FTPDIR}/
 cd ${FTPDIR}/
 /usr/bin/md5sum ${RPMFL} > ${RPMFL}.md5

 # Make ${INFOout} Report For Mailing

 if [ ! -f ${INFOout} ]; then
  echo " " > ${INFOout}
  echo "On: ${YADAT}  " >> ${INFOout}
  echo " " >> ${INFOout}
  echo "(output in: ${FTPDIR})" >> ${INFOout}
  echo " " >> ${INFOout}
  echo "Scintilla build run on: ${MACHON} " >> ${INFOout}
  cat /etc/redhat-release  >> ${INFOout}
  echo "running Kernel: ${KRNREL} used: " >> ${INFOout}
  echo "=============================== " >> ${INFOout}
  # Grab Some Build Info Since We Use Local Defaults:
  rpm -q gcc >> ${INFOout}
  rpm -q glibc | head -1 >> ${INFOout}
  rpm -q make >> ${INFOout}
  rpmbuild --version >> ${INFOout}
  git --version >> ${INFOout}
  echo " " >> ${INFOout}
  echo "${i} build available" >> ${INFOout}
  echo "binary size: ${SIZ}" >> ${INFOout}
 else
  echo " " >> ${INFOout}
  echo "${i} build available" >> ${INFOout}
  echo "binary size: ${SIZ}" >> ${INFOout}
  echo " " >> ${INFOout}
 fi

done

# Ensure Source Tagged With Version (& Pushed Up)

# cvs tag BLD_ETHINZA_${VRS}
    git tag BUILD_ETHINZA_${VRS}

# cvs push --tags
    git push --tags
echo " "

