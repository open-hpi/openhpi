##################################################
#
#  OpenHPI Macros
#
#  Copyright (C) IBM Corp 2003
#
#  Author(s):
#      Sean Dague <http://dague.net/sean>
#
#  This file is licensed under the same terms as OpenHPI itself.
#  See the COPYING file in the top level of OpenHPI for more info.
#
# This is a set of common macros to be used by the top
# level configure.in.
#
##################################################


AC_DEFUN(OH_SET_SIZES,
    [
    OH_SSFILE=testsize
    OH_SSSOURCE="$OH_SSFILE.c"
    echo "#include <stdlib.h>" > $OH_SSSOURCE
    echo "#include <stdio.h>" >> $OH_SSSOURCE
    echo "int main() {" >> $OH_SSSOURCE
    # add more here if you need them
    # the lots of slashes are needed to do the processing below right
    echo "printf(\"unsigned char %d\\\\n\",sizeof(unsigned char));" >> $OH_SSSOURCE
    echo "printf(\"unsigned short %d\\\\n\",sizeof(unsigned short));" >> $OH_SSSOURCE
    echo "printf(\"unsigned int %d\\\\n\",sizeof(unsigned int));" >> $OH_SSSOURCE
    echo "printf(\"char %d\\\\n\",sizeof(char));" >> $OH_SSSOURCE
    echo "printf(\"short %d\\\\n\",sizeof(short));" >> $OH_SSSOURCE
    echo "printf(\"int %d\\\\n\",sizeof(int));" >> $OH_SSSOURCE
    echo "printf(\"long long %d\\\\n\",sizeof(long long));" >> $OH_SSSOURCE
    echo "printf(\"float %d\\\\n\",sizeof(float));" >> $OH_SSSOURCE
    echo "printf(\"double %d\\\\n\",sizeof(double));" >> $OH_SSSOURCE
    echo "return 0;" >> $OH_SSSOURCE
    echo "}" >> $OH_SSSOURCE
    
    gcc -o $OH_SSFILE $OH_SSSOURCE

    OH_TYPE_SIZES=`./$OH_SSFILE`
    # feel free to define more logic here if we need it
    
    OH_SIZEOF_UCHAR=`echo -e $OH_TYPE_SIZES | grep "^unsigned char" | awk '{print $[3]}'`
    OH_SIZEOF_USHORT=`echo -e $OH_TYPE_SIZES | grep "^unsigned short" | awk '{print $[3]}'`
    OH_SIZEOF_UINT=`echo -e $OH_TYPE_SIZES | grep "^unsigned int" | awk '{print $[3]}'`
    OH_SIZEOF_CHAR=`echo -e $OH_TYPE_SIZES | grep "^char" | awk '{print $[2]}'`
    OH_SIZEOF_SHORT=`echo -e $OH_TYPE_SIZES | grep "^short" | awk '{print $[2]}'`
    OH_SIZEOF_INT=`echo -e $OH_TYPE_SIZES | grep "^int" | awk '{print $[2]}'`
    OH_SIZEOF_LLONG=`echo -e $OH_TYPE_SIZES | grep "^long long" | awk '{print $[3]}'`
    OH_SIZEOF_FLOAT=`echo -e $OH_TYPE_SIZES | grep "^float" | awk '{print $[2]}'`
    OH_SIZEOF_DOUBLE=`echo -e $OH_TYPE_SIZES | grep "^double" | awk '{print $[2]}'`
    rm -f $OH_SSFILE $OH_SSSOURCE
    ])

#
# OH_CHECK_FAIL($LIBNAME,$PACKAGE_SUGGEST,$URL,$EXTRA)
#

AC_DEFUN(OH_CHECK_FAIL,
    [
    OH_MSG=`echo -e "- $1 not found!\n"`
    if test "x" != "x$4"; then
        OH_MSG=`echo -e "$OH_MSG\n- $4"`
    fi
    if test "x$2" != "x"; then
        OH_MSG=`echo -e "$OH_MSG\n- Try installing the $2 package\n"`
    fi
    if test "x$3" != "x"; then
        OH_MSG=`echo -e "$OH_MSG\n- or get the latest software from $3\n"`
    fi
    
    AC_MSG_ERROR(
!
************************************************************
$OH_MSG
************************************************************
)
    ]
)

# it is worth noting that we have to strip 
# optimization from the cflags for net-snmp
# hopefully they'll fix that bug in the future

AC_DEFUN(OH_CHECK_NETSNMP,
    [
    AC_MSG_CHECKING(for net-snmp)
    AC_TRY_LINK(
    [
        #include <net-snmp/net-snmp-config.h>
        #include <net-snmp/net-snmp-includes.h>
    ],
    [
        struct snmp_session session
    ],
    [
        have_netsnmp=yes
        SNMPFLAGS=`net-snmp-config --cflags | perl -p -e 's/-O\S*//g'`
        SNMPLIBS=`net-snmp-config --libs`
        SNMPDIR=snmp
        AC_MSG_RESULT(yes)
    ],
    [AC_MSG_RESULT(no.  No SNMP based plugins can be built!)])
])
