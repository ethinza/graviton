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

# Clean Build Source Of Non-Source Files And Directories
#

echo " "  
echo "WARNING!! This Step Will Try To Remove Any Files NOT Under Source Control." 

# Use SCM To Look For Uncontrolled Files/Directories
clny()
{
#cvs clean -xf  
git clean -xf
}
echo " "  


if [ "${QUI}" = "rbutler" ] || [ "${QUI}" = "ibBuilds" ]; then
	clny  
else
	echo " "  
	echo "Proceed And You Will REMOVE Any Files NOT Under Source Control" 
	echo "Do You Really Want To Execute Clobber?"  
	echo " "
	echo "If So, Please Reply: yes or no." 
	read REPLY
	echo " "
	case ${REPLY} in
		y | Y | YES | Yes | yes) clny ;;
		*) echo "O.K. Will Not Execute Clobber Target"
		;;
	esac
		
fi

echo " "
