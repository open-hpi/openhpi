Name: hpiutil
%define Version 1.0.4
Version: %Version
Release: 2
Summary: Contains HPI server management utilities and library.
Copyright: BSD
Group: System Environment/Kernel
Source: hpiutil-%Version.tar.gz
%ifarch x86_pentium3
AutoReqProv: No
%endif
%ifarch x86_pentium4
AutoReqProv: No
%endif

%description
The HPI utilities package provides system management utilities that 
conform to the SA Forum's Hardware Platform Interface specification, and
as such are hardware-independent across platforms that have an HPI
library implementation.  The HPI library on Intel platforms requires
an IPMI driver.  An IPMI driver can be provided by the OpenIPMI driver 
(/dev/ipmi0) which is included in Linux kernel versions 2.4.20 and greater.

This package includes the following HPI utilities.
  hpisensor
  hpisel
  hpireset
  hpiwdt
  hpifru
  hpialarmpanel
and a test tool called hpievent. 

%prep
#%setup -q

#%build
#sh configure
#make

#%install
#RPM_BUILD_ROOT=`pwd`
#make DESTDIR=${RPM_BUILD_ROOT} install
#( cd ${RPM_BUILD_ROOT}/usr/man/man8; gzip -f *.8 ) 

%files
%defattr(0755,root,root)
/usr/bin/hpifru
/usr/bin/hpisensor
/usr/bin/hpialarmpanel
/usr/bin/hpisel
/usr/bin/hpiwdt
/usr/bin/hpireset
/usr/share/hpiutil/hpievent
/usr/share/hpiutil/env.openhpi
%defattr(0664,root,root)
/usr/share/hpiutil/README
/usr/share/hpiutil/COPYING
# %defattr(-,root,root)
# %doc README TODO COPYING ChangeLog

%pre
# before install
sdir=/usr/share/hpiutil
echo "Installing HPI Utilities ..."
mkdir -p $sdir

# Check for an IPMI driver 
rpm -qa |grep ipmidrvr >/dev/null
if [ $? -ne 0 ]
then
  # Intel ipmidrvr package is not installed, but other IPMI drivers 
  # could also be used, so test for device files.
  dev1=/dev/imb
  dev2=/dev/ipmi0
  dev3=/dev/ipmi/0
  dev4=/dev/ipmikcs
  if [ ! -c $dev1 ]
  then
    if [ ! -c $dev2 ]
    then
       if [ ! -c $dev3 ]
       then
          echo "WARNING: No IPMI devices found ($dev1, $dev2 or $dev3)."
          echo "The HPI utilities depend on an IPMI driver. "
       fi
    fi
  fi
fi

%post
# after install
sdir=/usr/share/hpiutil

echo "hpiutil install started `date`" 
# Assumes that the kernel modules are already in place.

echo "done `date`"

%preun
# before uninstall
echo "Uninstalling HPI Utilities feature ..."

%postun
# after uninstall, clean up anything left over
sdir=/usr/share/hpiutil
tmped=/tmp/edmk.tmp

rm -rf $sdir  2>/dev/null

%changelog
* Fri Jul 24 2003 Andrew Cress <arcress@users.sourceforge.net> 
- modified for OpenIPMI/OpenHPI conditions
* Fri Jun 27 2003 Andrew Cress <arcress@users.sourceforge.net> 
- updated to check for ipmidrvr rpm, since no /dev/imb until reboot.
* Fri Jun 20 2003 Andrew Cress <arcress@users.sourceforge.net> 
- updated for README & released file locations
* Thu Jun 12 2003 Andrew Cress <arcress@users.sourceforge.net> 
- updated for beta2 file naming
* Tue May 05 2003 Andrew Cress <arcress@users.sourceforge.net> 
- created
