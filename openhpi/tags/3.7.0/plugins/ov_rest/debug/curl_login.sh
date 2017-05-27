#
# (C) Copyright 2016 Hewlett Packard Enterprise Development LP
#                    All rights reserved.
#
# Redistribution and use in source and binary forms, with or
# without modification, are permitted provided that the following
# conditions are met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in
# the documentation and/or other materials provided with the distribution.
#
# Neither the name of the Hewlett Packard Enterprise, nor the names
# of its contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#!/bin/bash
######################################################################################
# curl_login.sh
#
# This tool just tests whether the login works at all or not. Many default values are
# set which could be over-written with command line options. A quick test if the
# openhpid will work or not
# The following are the options that could be given
# system - IP or name
# user - username
# pass - password
# xapi - xapi version
#
# Return - session ID or login failure
#
######################################################################################

# Initialize the default values
SYSTEM=""
XAPI=300
USER=Administrator
PASS=pass
SID=""
VER=0
LOG=/tmp/login_output.txt
ENC=/tmp/enclosures.txt

# Check if the things are good to go
which curl >/dev/null 2>&1
if [ $? -ne 0 ]
then
	echo "curl command is not installed. Exiting".
	exit 1
fi

usage()
{
        echo "$0 [-u user] [ -p password ] [-x xapi] [-v] -s system"
	echo "	-s system IP or name"
	echo "	-u user username (default: Administrator)"
	echo "	-p pass password (default: pass)"
	echo "	-x xapi X-Api-Version (default: 300)"
	echo "	-v Verbose (default: off)"
	exit 2
}


# Get the option
# set -- $(getopt s:u:p:x:v "$@")
while getopts ":s:u:p:x:v" opt
do
    case $opt in
    s) SYSTEM=$OPTARG; 
	;;
    u) USER=$OPTARG; 
	;;
    p) PASS=$OPTARG; 
	;;
    x) XAPI=$OPTARG;
	;;
    v) VER=1; 
	;;
    \?) echo "$0: error - unrecognized option -$OPTARG" >&2 
	usage 
	exit 1
	;;
    :) echo "Option -$OPTARG requires an argument." >&2
	usage
	exit 1
	;;
    esac
done

if [ ${SYSTEM:-none} == "none" ]
then
	echo "System IP or name must be provided"
	usage
fi

rm -f $LOG $ENC

if [ $VER -eq 1 ]
then
	echo "curl -i -k -H \"Accept:application/json\" -H \"Content-Type:application/json\" \
        -H \"X-API-Version:$XAPI\" -X POST -d \
        '{\"userName\":\"$USER\",\"password\":\"$PASS\",\"authLoginDomain\":\"\"}' \
        https://${SYSTEM}/rest/login-sessions"
fi

curl -i -k -H "Accept:application/json" -H "Content-Type:application/json" \
	-H "X-API-Version:$XAPI" -X POST \
	-d '{"userName":"'$USER'","password":"'$PASS'","authLoginDomain":""}' \
	https://${SYSTEM}/rest/login-sessions  >$LOG 2>&1
grep sessionID ${LOG} >/dev/null
if [ $? -eq 0 ]
then
	SID=`cat $LOG | grep sessionID | awk -F"sessionID" ' { print $2 }' | awk -F"\"" ' { print $3 }'`
	echo "Session ID is $SID"
else
	echo "curl call to login failed. Error is"
	cat $LOG | grep errorCode | awk -F"errorCode" ' { print $2 }' | awk -F"\"" ' { print $3 }' 
	exit 3
fi
	
# Check if the SID could be used to get the data
curl -k -H "Accept:application/json" -H "Content-Type:application/json" \
	-H "X-API-Version:$XAPI" -H "auth:$SID" -X GET https://${SYSTEM}/rest/enclosures > $ENC 2>&1
NO_CHARS=`cat $ENC | wc -c`
if [ $NO_CHARS -gt 5000 ]
then
	echo "Looks like the sessionID works well"
else
	echo "Looks like sessionID may be wrong. Got only $NO_CHARS charactors"
fi




