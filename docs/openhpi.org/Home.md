
Welcome
=======

<span id="line-3" class="anchor"></span><span id="line-4"
class="anchor"></span>

OpenHPI provides an open source implementation of the [Service
Availability Forum](http://saforum.org) (SAF) Hardware Platform
Interface (HPI). <span id="line-5" class="anchor"></span><span
id="line-6" class="anchor"></span>

HPI is an abstracted interface for managing computer hardware, typically
chassis and rack based servers. HPI includes resource modeling; access
to and control over sensor, control, watchdog, and inventory data
associated with resources; abstracted System Event Log interfaces;
hardware events and alarms; and a managed hotswap interface. <span
id="line-7" class="anchor"></span><span id="line-8"
class="anchor"></span>

OpenHPI's architecture contains a modular mechanism intended to make
adding new hardware support easier. Several plugins exist in the OpenHPI
source tree giving access to various types of hardware. This includes,
but is not limited to, IPMI based servers, Blade Center, and machines
which export data via sysfs. <span id="line-9"
class="anchor"></span><span id="line-10" class="anchor"></span>

The OpenHPI project is always looking for more participants. If you are
interested in using and/or taking a part in the evolution of OpenHPI,
then [contact us](Contact_Us) and we'll tell you how. <span id="line-11"
class="anchor"></span><span id="line-12" class="anchor"></span>

**About this wiki**\
<span id="line-13" class="anchor"></span>This wiki is being graciously
hosted by [OSDL](http://www.osdl.org) at the Marist College OSDL
laboratory using a zLinux image.\
<span id="line-14" class="anchor"></span>To use this wiki, you must
[sign up](http://openhpi.org/UserPreferences), create a
valid user id and contact [me](BryanSutula) (a spam prevention
technique) so that you can create and edit pages. If you want to learn
more about using a wiki, check out the
[HelpContents](http://openhpi.org/HelpContents) page.
<span id="line-15" class="anchor"></span><span id="line-16"
class="anchor"></span>

**OpenHPI has been moved to [github](https://github.com/open-hpi) from [sourceforge](https://sourceforge.net/projects/openhpi/) as of May 2, 2018** 
<span id="line-17" class="anchor">

</span><span id="line-18" class="anchor"></span>

### Downloads

<span id="line-19" class="anchor">
</span><span id="line-20"class="anchor"></span>

<div id="Downloads-1.content" dir="ltr" lang="en">

<span id="Downloads-1.top" class="anchor"></span> <span
id="Downloads-1.line-1" class="anchor"></span>

[Github Project Page](https://github.com/open-hpi),
 [SourceForge
Project Page](http://sourceforge.net/projects/openhpi), 
[Linux distros that carry OpenHPI](Distributions) <span id="Downloads-1.line-2"
class="anchor"></span>

<div>



| Module| Latest Release | Status | Description | 
|--|--| -- | -- | 
| **OpenHPI**| [3.8.0](https://sourceforge.net/projects/openhpi/files/openhpi-stable/3.8.0/) (Stable) |supported |The open implementation of HPI. Includes a library and a daemon. | 
| **HPI Browser** | [3.1.0](https://sourceforge.net/projects/openhpi/files/hpibrowser/3.1.0/) (Development) |supported |HPI-B.03.02 GUI application based on QT4. Best with OpenHPI-3.1.0| 
| **SNMP Subagent**  |[2.3.4](http://prdownloads.sourceforge.net/openhpi/openhpi-subagent-2.3.4.tar.gz?download)  (Stable) |unsupported | Exports the HPI API using the [SAF](http://saforum.org/) SNMP MIBs.|
|**HPIView** | [2.0](http://prdownloads.sourceforge.net/openhpi/hpiview-2.0.tar.gz?download) (Stable)|unsupported| OpenHPI GUI application based on GTK.|
|**Python Module**  | [1.1](http://downloads.sourceforge.net/openhpi/py-openhpi-1.1.tar.gz?use_mirror=osdn)  (Stable)| unsupported |OpenHPI GUI application based on GTK.|
</div>
<span id="Downloads-1.line-7" class="anchor"></span><span
id="Downloads-1.line-8" class="anchor"></span>

Note: Unsupported portions of the project may be quite out of date and
would likely require work in order to function. If you are interested in
one of these, please consider submitting patches or volunteering to
maintain it. See [Developers](Developers) <span id="Downloads-1.line-9"
class="anchor"></span><span id="Downloads-1.bottom"
class="anchor"></span>
</div>

<span id="line-21" class="anchor"></span><span id="line-22"
class="anchor"></span>

Latest News
===========

<span id="line-23" class="anchor"></span><span id="line-24"
class="anchor"></span>

<div id="LatestNews.content" dir="ltr" lang="en">

<span id="LatestNews.top" class="anchor"></span> <span
id="LatestNews.line-1" class="anchor"></span>

##### OpenHPI 3.8.0 has been released! (03/09/2018) 

<span id="LatestNews.line-2" class="anchor"></span>
-   Stable release <span id="LatestNews.line-3" class="anchor"></span>
-   Changes to many plugins, build, utils, clients and daemon <span
    id="LatestNews.line-4" class="anchor"></span>
-   Lot of bugfixes <span id="LatestNews.line-5" class="anchor"></span>

Details can be found at the complete 3.8.0
[changelog](Changelogs/3.8.0.md). <span id="LatestNews.line-6"
class="anchor"></span><span id="LatestNews.line-7"
class="anchor"></span>

##### OpenHPI 3.7.0 has been released! (05/26/2017) 

<span id="LatestNews.line-8" class="anchor"></span>
-   Development release <span id="LatestNews.line-9"
    class="anchor"></span>
-   New ov\_rest plugin added <span id="LatestNews.line-10"
    class="anchor"></span>
-   Many bugfixes <span id="LatestNews.line-11" class="anchor"></span>

Details can be found at the complete 3.7.0
[changelog](Changelogs/3.7.0.md). <span id="LatestNews.line-12"
class="anchor"></span><span id="LatestNews.line-13"
class="anchor"></span>

##### OpenHPI 3.6.1 has been released! (09/15/2015)

<span id="LatestNews.line-14" class="anchor"></span>
-   Stable patch release <span id="LatestNews.line-15"
    class="anchor"></span>
-   Makefile fix, bootstrap information and hpithres fixes <span
    id="LatestNews.line-16" class="anchor"></span>

Details can be found at the complete 3.6.1
[changelog](Changelogs/3.6.1.md). <span id="LatestNews.line-17"
class="anchor"></span><span id="LatestNews.line-18"
class="anchor"></span>

##### OpenHPI 3.6.0 has been released! (08/26/2015)

<span id="LatestNews.line-19" class="anchor"></span>
-   Stable release <span id="LatestNews.line-20" class="anchor"></span>
-   /var/log/openhpi security fix <span id="LatestNews.line-21"
    class="anchor"></span>
-   Fixes for new OS Releases, new GLIB, ssl etc <span
    id="LatestNews.line-22" class="anchor"></span>
-   Many bugfixes <span id="LatestNews.line-23" class="anchor"></span>

Details can be found at the complete 3.6.0
[changelog](Changelogs/3.6.0.md). <span id="LatestNews.line-24"
class="anchor"></span><span id="LatestNews.line-25"
class="anchor"></span>

##### OpenHPI 3.5.0 has been released! (10/09/2014)

<span id="LatestNews.line-26" class="anchor"></span>
-   Development release <span id="LatestNews.line-27"
    class="anchor"></span>
-   Changelog added to release <span id="LatestNews.line-28"
    class="anchor"></span>
-   Scripts/Files for new sourceforge <span id="LatestNews.line-29"
    class="anchor"></span>
-   Many bugfixes <span id="LatestNews.line-30" class="anchor"></span>

Details can be found at the complete 3.5.0
[changelog](Changelogs/3.5.0.md). <span id="LatestNews.line-31"
class="anchor"></span><span id="LatestNews.line-32"
class="anchor"></span>

##### OpenHPI 3.4.0 has been released! (11/14/2013) 

<span id="LatestNews.line-33" class="anchor"></span>
-   Stable release <span id="LatestNews.line-34" class="anchor"></span>
-   uid\_map file security fix <span id="LatestNews.line-35"
    class="anchor"></span>
-   Lot of bugfixes <span id="LatestNews.line-36" class="anchor"></span>

Details can be found at the complete 3.4.0
[changelog](Changelogs/3.4.0.md). <span
id="LatestNews.line-37" class="anchor"></span><span
id="LatestNews.line-38" class="anchor"></span>

##### OpenHPI 3.2.1 has been released! (07/18/2013) 

<span id="LatestNews.line-39" class="anchor"></span>
-   Stable release <span id="LatestNews.line-40" class="anchor"></span>
-   Config file security fix <span id="LatestNews.line-41"
    class="anchor"></span>
-   Lot of bugfixes <span id="LatestNews.line-42" class="anchor"></span>

Details can be found at the complete 3.2.1
[changelog](Changelogs/3.2.1.md). <span
id="LatestNews.line-43" class="anchor"></span><span
id="LatestNews.line-44" class="anchor"></span>

##### OpenHPI 3.2.0 has been released! (08/27/2012) 

<span id="LatestNews.line-45" class="anchor"></span>
-   Stable release <span id="LatestNews.line-46" class="anchor"></span>
-   hpi\_shell for Windows <span id="LatestNews.line-47"
    class="anchor"></span>
-   Test Agent plug-in supports DIMI and FUMI <span
    id="LatestNews.line-48" class="anchor"></span>
-   Lot of bugfixes <span id="LatestNews.line-49" class="anchor"></span>

Details can be found at the complete 3.2.0
[changelog](Changelogs/3.2.0.md). <span
id="LatestNews.line-50" class="anchor"></span><span
id="LatestNews.line-51" class="anchor"></span>

##### HpiBrowser 3.1.0 has been released! (05/21/2012) 

<span id="LatestNews.line-52" class="anchor"></span>
-   Windows support <span id="LatestNews.line-53" class="anchor"></span>
-   GUI for selecting HPI domain to connect <span
    id="LatestNews.line-54" class="anchor"></span>
-   Bugfixes <span id="LatestNews.line-55" class="anchor"></span>

Details can be found at the complete 3.1.0
[changelog](Changelogs/HpiBrowser-3.1.0.md). <span
id="LatestNews.line-56" class="anchor"></span><span
id="LatestNews.line-57" class="anchor"></span>

##### OpenHPI 3.1.0 has been released! (05/01/2012) 

<span id="LatestNews.line-58" class="anchor"></span>
-   Base libraries for C\#/Java/Python <span id="LatestNews.line-59"
    class="anchor"></span>
-   Lot of bugfixes <span id="LatestNews.line-60" class="anchor"></span>

Details can be found at the complete 3.1.0
[changelog](Changelogs/3.1.0.md). <span
id="LatestNews.line-61" class="anchor"></span><span
id="LatestNews.line-62" class="anchor"></span>

##### OpenHPI 3.0.0 has been released! (10/17/2011) 

<span id="LatestNews.line-63" class="anchor"></span>
-   Documentation updates <span id="LatestNews.line-64"
    class="anchor"></span>
-   Bugfixes <span id="LatestNews.line-65" class="anchor"></span>

Details can be found at the complete 3.0.0
[changelog](Changelogs/3.0.0.md). <span
id="LatestNews.line-66" class="anchor"></span><span
id="LatestNews.line-67" class="anchor"></span>

##### OpenHPI 2.17.0 has been released! (06/20/2011) 

<span id="LatestNews.line-68" class="anchor"></span>
-   Refactoring <span id="LatestNews.line-69" class="anchor"></span>
-   Windows support <span id="LatestNews.line-70" class="anchor"></span>
-   FreeBSD support <span id="LatestNews.line-71" class="anchor"></span>
-   IPv6 support <span id="LatestNews.line-72" class="anchor"></span>
-   New Test Agent plug-in <span id="LatestNews.line-73"
    class="anchor"></span>
-   New hpixml client <span id="LatestNews.line-74"
    class="anchor"></span>
-   More bugfixes and features <span id="LatestNews.line-75"
    class="anchor"></span>

Details can be found at the complete 2.17.0
[changelog](Changelogs/2.17.0.md). <span
id="LatestNews.line-76" class="anchor"></span><span
id="LatestNews.line-77" class="anchor"></span>

##### OpenHPI 2.16.0 has been released! (02/23/2011) 

<span id="LatestNews.line-78" class="anchor"></span>
-   Documentation updates <span id="LatestNews.line-79"
    class="anchor"></span>
-   Re-issuing HPI call in case of broken connection (can happen for
    configuration with redundant OpenHPI daemons) <span
    id="LatestNews.line-80" class="anchor"></span>

Details can be found at the complete 2.16.0
[changelog](Changelogs/2.16.0.md). <span
id="LatestNews.line-81" class="anchor"></span><span
id="LatestNews.line-82" class="anchor"></span>

##### OpenHPI for Windows One more Preview snapshot (01/16/2011) 

<span id="LatestNews.line-83" class="anchor"></span>
-   SVN trunk shapshot, revision 7245, at Jan 16, 2011 <span
    id="LatestNews.line-84" class="anchor"></span>
-   Base Library, Clients, OpenHPI daemon as a console application and
    Slave plug-in <span id="LatestNews.line-85" class="anchor"></span>
-   Some guidance how to use it in README.windows file <span
    id="LatestNews.line-86" class="anchor"></span>
-   Library names do not contain version now. For example libopenhpi.dll
    <span id="LatestNews.line-87" class="anchor"></span>
-   Introduced two private side-by-side assemblies: for libraries and
    for plug-in <span id="LatestNews.line-88" class="anchor"></span>
-   Necessary glib libraries were included <span id="LatestNews.line-89"
    class="anchor"></span>

##### OpenHPI for Windows One more Preview snapshot (01/08/2011) 

<span id="LatestNews.line-90" class="anchor"></span>
-   SVN trunk shapshot, revision 7239, at Jan 08, 2011 <span
    id="LatestNews.line-91" class="anchor"></span>
-   Base Library, Clients, OpenHPI daemon as a console application and
    Slave plug-in <span id="LatestNews.line-92" class="anchor"></span>
-   Some guidance how to use it in README.windows file <span
    id="LatestNews.line-93" class="anchor"></span>

##### OpenHPI for Windows Preview snapshot (12/18/2010) 

<span id="LatestNews.line-94" class="anchor"></span>
-   SVN trunk shapshot, revision 7209, at Dec 18, 2010 <span
    id="LatestNews.line-95" class="anchor"></span>
-   Base Library and Clients <span id="LatestNews.line-96"
    class="anchor"></span>
-   Some guidance how to use it in README.windows file <span
    id="LatestNews.line-97" class="anchor"></span>

##### OpenHPI 2.15.1 has been released! (12/13/2010) 

<span id="LatestNews.line-98" class="anchor"></span>
-   Development release <span id="LatestNews.line-99"
    class="anchor"></span>
-   Revisited OpenHPI API (oHpiXXX functions) <span
    id="LatestNews.line-100" class="anchor"></span>
-   Refactoring in marshal, utils and baselib <span
    id="LatestNews.line-101" class="anchor"></span>
-   Slave - new plug-in for representing resources and instruments from
    subsidiary OpenHPI daemon <span id="LatestNews.line-102"
    class="anchor"></span>
-   Bugfixes <span id="LatestNews.line-103" class="anchor"></span>

Details can be found at the complete 2.15.1
[changelog](Changelogs/2.15.1.md). <span
id="LatestNews.line-104" class="anchor"></span><span
id="LatestNews.line-105" class="anchor"></span>

##### OpenHPI 2.15.1 is planned for December 2010 release 

<span id="LatestNews.line-106" class="anchor"></span>
-   Development release <span id="LatestNews.line-107"
    class="anchor"></span>
-   Revisited OpenHPI API (oHpiXXX functions) <span
    id="LatestNews.line-108" class="anchor"></span>
-   Refactoring in marshal, utils and baselib <span
    id="LatestNews.line-109" class="anchor"></span>
-   Slave - new plug-in for representing resources and instruments from
    subsidiary OpenHPI daemon <span id="LatestNews.line-110"
    class="anchor"></span>
-   Bugfixes <span id="LatestNews.line-111" class="anchor"></span>

##### OpenHPI 2.15.0 has been released! (06/30/2010) 

<span id="LatestNews.line-112" class="anchor"></span>
-   Development release <span id="LatestNews.line-113"
    class="anchor"></span>
-   Improved build structure <span id="LatestNews.line-114"
    class="anchor"></span>
-   New power management controls for HP c-Class enclosures <span
    id="LatestNews.line-115" class="anchor"></span>
-   New dynamic simulator plugin <span id="LatestNews.line-116"
    class="anchor"></span>
-   Many enhancements to the OpenHPI clients <span
    id="LatestNews.line-117" class="anchor"></span>
-   Approximately 30 defects fixed <span id="LatestNews.line-118"
    class="anchor"></span>

Details can be found at the complete 2.15.0
[changelog](Changelogs/2.15.0.md). <span
id="LatestNews.line-119" class="anchor"></span><span
id="LatestNews.line-120" class="anchor"></span>

##### OpenHPI 2.15.0 is targeting the week of June 28, 2010 for release 

<span id="LatestNews.line-121" class="anchor"></span>

As of June 17, 2010, we are now in the release cycle for OpenHPI 2.15.0
<span id="LatestNews.line-122" class="anchor"></span><span
id="LatestNews.line-123" class="anchor"></span>

-   Only defects and very low-risk enhancements allowed during period of
    June 14 - June 25. <span id="LatestNews.line-124"
    class="anchor"></span>
-   Release is targeted for the week of June 28. <span
    id="LatestNews.line-125" class="anchor"></span>
-   This is a development release of OpenHPI and includes new features:
    <span id="LatestNews.line-126" class="anchor"></span>
-   \* New dynamic simulator plug-in. <span id="LatestNews.line-127"
    class="anchor"></span>
-   \* oa\_soap plug-in enhancements for power management <span
    id="LatestNews.line-128" class="anchor"></span>
-   \* Client enhancements for domain support <span
    id="LatestNews.line-129" class="anchor"></span>
-   \* Build restructure <span id="LatestNews.line-130"
    class="anchor"></span>
-   \* Defect fixes <span id="LatestNews.line-131"
    class="anchor"></span>

##### OpenHPI 2.15.0 is tentatively planned for a mid-June/early-July 2010 release 

<span id="LatestNews.line-132" class="anchor"></span>

Some planned features for this release: <span id="LatestNews.line-133"
class="anchor"></span><span id="LatestNews.line-134"
class="anchor"></span>

-   Build restructure (Anton Pak - Pigeon Point) <span
    id="LatestNews.line-135" class="anchor"></span>
-   oa\_soap plugin power-management controls (Michael Bishop - HP)
    <span id="LatestNews.line-136" class="anchor"></span>
-   Possible new simulator plugin (Lars Wetzel - freelance developer)
    <span id="LatestNews.line-137" class="anchor"></span>
-   (Out: Marking the OpenIPMI plugin code as deprecated. Objection from
    Camiant.com - Shaheen Ali) <span id="LatestNews.line-138"
    class="anchor"></span>
-   Bug fixes <span id="LatestNews.line-139" class="anchor"></span>

For other 2.15.0 features/enhancements - and to discuss the release
schedule - please post on the developer's mailing list:
<openhpi-devel@lists.sourceforge.net> <span id="LatestNews.line-140"
class="anchor"></span><span id="LatestNews.line-141"
class="anchor"></span>

##### OpenHPI 2.14.1 has been released! (11/23/2009) 

<span id="LatestNews.line-142" class="anchor"></span>
-   Fixes to the OpenHPI Daemon and base library. <span
    id="LatestNews.line-143" class="anchor"></span>
-   Many bug fixes to the HPI Shell. <span id="LatestNews.line-144"
    class="anchor"></span>
-   Fixes for building on Solaris. <span id="LatestNews.line-145"
    class="anchor"></span>
-   Fixes to the HP c-Class, HP ProLiant, IPMI Direct, and SNMP
    Client plug-ins. <span id="LatestNews.line-146"
    class="anchor"></span>
-   Numerous other bug fixes. <span id="LatestNews.line-147"
    class="anchor"></span>

Details can be found at the complete 2.14.1
[changelog](Changelogs/2.14.1.md). <span
id="LatestNews.line-148" class="anchor"></span><span
id="LatestNews.line-149" class="anchor"></span>

##### OpenHPI 2.14.0 has been released! (04/16/2009) 

<span id="LatestNews.line-150" class="anchor"></span>
-   Content from development releases 2.13.0 through 2.13.3 <span
    id="LatestNews.line-151" class="anchor"></span>
-   Many bug fixes to the HPI Shell. <span id="LatestNews.line-152"
    class="anchor"></span>
-   Fixes for building on Mandrivia. <span id="LatestNews.line-153"
    class="anchor"></span>
-   Numerous other bug fixes. <span id="LatestNews.line-154"
    class="anchor"></span>

Details can be found at the complete 2.14.0
[changelog](Changelogs/2.14.0.md). <span
id="LatestNews.line-155" class="anchor"></span><span
id="LatestNews.line-156" class="anchor"></span>

##### OpenHPI 2.13.3 has been released! (02/24/2009) 

<span id="LatestNews.line-157" class="anchor"></span>
-   HPI-B.03.01 support. <span id="LatestNews.line-158"
    class="anchor"></span>
-   HPI Shell: Severity fix for announcements added to annunciators.
    <span id="LatestNews.line-159" class="anchor"></span>
-   OpenHPI Daemon: Hysteresis values are now validated correctly. <span
    id="LatestNews.line-160" class="anchor"></span>
-   Numerous bug fixes. <span id="LatestNews.line-161"
    class="anchor"></span>

Details can be found at the complete 2.13.3
[changelog](Changelogs/2.13.3.md). <span
id="LatestNews.line-162" class="anchor"></span><span
id="LatestNews.line-163" class="anchor"></span>

##### OpenHPI 2.13.2 has been released! (1/16/2009) 

<span id="LatestNews.line-164" class="anchor"></span>
-   HP c-Class plugin: additional sensors. <span
    id="LatestNews.line-165" class="anchor"></span>
-   Resolved a compile problem on SPARC. <span id="LatestNews.line-166"
    class="anchor"></span>
-   Added hpi\_shell command to obtain version information <span
    id="LatestNews.line-167" class="anchor"></span>
-   Numerous bug fixes <span id="LatestNews.line-168"
    class="anchor"></span>

Details can be found at the complete 2.13.2
[changelog](Changelogs/2.13.2.md). <span
id="LatestNews.line-169" class="anchor"></span><span
id="LatestNews.line-170" class="anchor"></span>

##### OpenHPI 2.13.1 has been released! (11/19/2008) 

<span id="LatestNews.line-171" class="anchor"></span>
-   HP c-Class plugin: add underpinnings for additional
    management functions. <span id="LatestNews.line-172"
    class="anchor"></span>
-   Add man pages for sample openhpi applications. <span
    id="LatestNews.line-173" class="anchor"></span>
-   Fix domain ID reporting in redundant domains with multiple
    daemon connections. <span id="LatestNews.line-174"
    class="anchor"></span>
-   Fix installation of openhpid initscript. <span
    id="LatestNews.line-175" class="anchor"></span>
-   HPI Shell: fix closing session and data display. <span
    id="LatestNews.line-176" class="anchor"></span>

Details can be found at the complete 2.13.1
[changelog](Changelogs/2.13.1.md). <span
id="LatestNews.line-177" class="anchor"></span><span
id="LatestNews.line-178" class="anchor"></span>

##### OpenHPI 2.13.0 has been released! (10/10/2008) 

<span id="LatestNews.line-179" class="anchor"></span>
-   HP c-class-Enhancement to add IO and Storage blade support. <span
    id="LatestNews.line-180" class="anchor"></span>
-   Enable redundant domains with multiple daemon connections. <span
    id="LatestNews.line-181" class="anchor"></span>
-   Add saHpiEventLogCapabilitiesGet to simulator plugin. <span
    id="LatestNews.line-182" class="anchor"></span>
-   Add entries for HP c-Class plugin pdf documents in Makefile.am.
    <span id="LatestNews.line-183" class="anchor"></span>
-   Support chage in the OA switchover behaviour. <span
    id="LatestNews.line-184" class="anchor"></span>

Details can be found at the complete 2.13.0
[changelog](Changelogs/2.13.0.md). <span
id="LatestNews.line-185" class="anchor"></span><span
id="LatestNews.line-186" class="anchor"></span>

##### OpenHPI 2.12.0 has been released! (07/25/2008) 

<span id="LatestNews.line-187" class="anchor"></span>
-   Rollup development releases 2.11.0 through 2.11.3. <span
    id="LatestNews.line-188" class="anchor"></span>
-   Add documentation for new ilo2\_ribcl and oa\_soap plugins HP
    [ProLiant](http://openhpi.org/ProLiant) plugin <span
    id="LatestNews.line-189" class="anchor"></span>
-   Make use of common SSL code HP c-Class Plugin. <span
    id="LatestNews.line-190" class="anchor"></span>
-   Various bug fixes. <span id="LatestNews.line-191"
    class="anchor"></span>

Details can be found at the complete 2.12.0
[changelog](Changelogs/2.12.0.md). <span
id="LatestNews.line-192" class="anchor"></span><span
id="LatestNews.line-193" class="anchor"></span>

##### OpenHPI 2.11.3 has been released! (06/25/2008) 

<span id="LatestNews.line-194" class="anchor"></span>
-   Add Dimis and Fumis to Simulator plugin. <span
    id="LatestNews.line-195" class="anchor"></span>
-   Fix invalid handling of ATCA Led Controls in Manual Mode, IPMI
    Direct plugin. <span id="LatestNews.line-196" class="anchor"></span>

Details can be found at the complete 2.11.3
[changelog](Changelogs/2.11.3.md). <span
id="LatestNews.line-197" class="anchor"></span><span
id="LatestNews.line-198" class="anchor"></span>

##### OpenHPI 2.11.2 has been released! (05/26/2008)

<span id="LatestNews.line-199" class="anchor"></span>

-   Important enhancements and many bug fixes to the HP c-Class plugin
    <span id="LatestNews.line-200" class="anchor"></span>
-   RTAS plugin build fixes <span id="LatestNews.line-201"
    class="anchor"></span>
-   Cross compilation build improvement regarding the number
    size checks. <span id="LatestNews.line-202" class="anchor"></span>
-   Obscure bug fixed in the daemon - affected ipmi plugin when it
    didn't find shelf manager initially <span id="LatestNews.line-203"
    class="anchor"></span>
-   Fixed persisted DAT issue - blank alarms <span
    id="LatestNews.line-204" class="anchor"></span>

Details can be found at the complete 2.11.2
[changelog](Changelogs/2.11.2.md). <span
id="LatestNews.line-205" class="anchor"></span><span
id="LatestNews.line-206" class="anchor"></span>

##### OpenHPI 2.11.1 has been released! (04/22/2008) 

<span id="LatestNews.line-207" class="anchor"></span>

This release adds the following highlighted features: <span
id="LatestNews.line-208" class="anchor"></span><span
id="LatestNews.line-209" class="anchor"></span>

-   Addition of HP Blade``System c-Class plugin <span
    id="LatestNews.line-210" class="anchor"></span>
-   Creates separate SSL support library for future modularity among
    plugins <span id="LatestNews.line-211" class="anchor"></span>
    -   added SSL library initialization to HPI initializaion <span
        id="LatestNews.line-212" class="anchor"></span>
-   Allows you to connect to multiple daemons from one client - The
    client library sees each daemon as a domain. This is in a beta state
    right now. To learn how to add domain/daemons or change the daemon
    used as the default domain, [join the mailing list and ask
    us](Contact_Us). <span id="LatestNews.line-213"
    class="anchor"></span>

It also comes with quite a number of bug fixes. Take a look at the
[changelog](Changelogs/2.11.1.md) for a complete list of
bug fixes. <span id="LatestNews.line-214" class="anchor"></span><span
id="LatestNews.line-215" class="anchor"></span>

##### OpenHPI 2.10.2 has been released! (03/03/2008) 

<span id="LatestNews.line-216" class="anchor"></span>

At long last the patch release to 2.10.1 has emerged! This release has
been a while in the making since there were a series of tough bugs that
needed fixing. A sack of bug fixes are part of 2.10.2 along with build
fixes for the SPARC platform (thanks crinaldo!). <span
id="LatestNews.line-217" class="anchor"></span><span
id="LatestNews.line-218" class="anchor"></span>

Following are some of the highlights: <span id="LatestNews.line-219"
class="anchor"></span><span id="LatestNews.line-220"
class="anchor"></span>

-   Various fixes to the rpm spec file thanks to reports on the mailing
    list <span id="LatestNews.line-221" class="anchor"></span>
-   Entity path related fixes to hpievents, hpi\_shell, and hpipower.
    <span id="LatestNews.line-222" class="anchor"></span>
-   Many fixes to hpi\_shell input mechanisms (thanks avpak!). <span
    id="LatestNews.line-223" class="anchor"></span>
-   Added missing sensor types to the utility library
    (thanks crinaldo!). <span id="LatestNews.line-224"
    class="anchor"></span>
-   IPMI Direct detection and hotswap fixes (by plugin
    mantainer, psangouard). <span id="LatestNews.line-225"
    class="anchor"></span>
-   Added transmission of missing hotswap event field
    (thanks martin\_volf!). <span id="LatestNews.line-226"
    class="anchor"></span>
-   Fixed transmission of variable-length arrays from client to
    daemon (renierm). <span id="LatestNews.line-227"
    class="anchor"></span>
-   Much improved DIMI/FUMI support (thanks avpak!). <span
    id="LatestNews.line-228" class="anchor"></span>
-   Fixed saHpiControlSet() hang and param check (thanks shuah!). <span
    id="LatestNews.line-229" class="anchor"></span>

Thanks to everyone else on the mailing list who reported bugs and
provided patches. <span id="LatestNews.line-230"
class="anchor"></span><span id="LatestNews.line-231"
class="anchor"></span>

Take a look at the [changelog](Changelogs/2.10.2.md) for
a complete list of bug fixes. <span id="LatestNews.line-232"
class="anchor"></span><span id="LatestNews.line-233"
class="anchor"></span>

##### OpenHPI 2.11.0 has been released (03/03/2008) 

<span id="LatestNews.line-234" class="anchor"></span>

2.11.0 includes all of the fixes in 2.10.2 plus a few features. Mainly:
<span id="LatestNews.line-235" class="anchor"></span><span
id="LatestNews.line-236" class="anchor"></span>

-   A new iLO2 RIBCL plug-in for managing HP
    [ProLiant](http://openhpi.org/ProLiant) Rack
    Mount servers. <span id="LatestNews.line-237" class="anchor"></span>
-   Enhancements to hpi\_shell including a command to reopen a session
    and to show a single inventory area (thanks to avpak). <span
    id="LatestNews.line-238" class="anchor"></span>
-   All HPI clients now report proper OpenHPI and SAF HPI version
    numbers to aid debugging of problems. <span id="LatestNews.line-239"
    class="anchor"></span>

Further enhancements in the 2.11.x branch will bring better SSL support
and the ability to connect to multiple daemons from one client. <span
id="LatestNews.line-240" class="anchor"></span><span
id="LatestNews.line-241" class="anchor"></span>

Take a look at the [changelog](Changelogs/2.11.0.md) for
a complete list of features and bug fixes. <span
id="LatestNews.line-242" class="anchor"></span><span
id="LatestNews.line-243" class="anchor"></span>

##### OpenHPI 2.10.1 has been released (11/02/2007) 

<span id="LatestNews.line-244" class="anchor"></span>

This release comes with several fixes to the build system, core library,
plugins, and clients. Everyone already on 2.10, should upgrade to this
latest version. Following, a summary of the most important bug fixes:
<span id="LatestNews.line-245" class="anchor"></span><span
id="LatestNews.line-246" class="anchor"></span>

-   saHpiGetIdByEntityPath() now returns the
    [InstrumentId](http://openhpi.org/InstrumentId) <span
    id="LatestNews.line-247" class="anchor"></span>
-   Solaris patches - Various fixes to make OpenHPI build cleanly on
    Solaris <span id="LatestNews.line-248" class="anchor"></span>
-   OpenIPMI plugin - storing the cause of a hotswap state change in the
    proper event byte <span id="LatestNews.line-249"
    class="anchor"></span>
-   hpiel, and other clients, no longer abort (possibly with a
    core dump) when the daemon is not present, but exit gracefully.
    <span id="LatestNews.line-250" class="anchor"></span>
-   The pristine tarball and a distcleaned tree now match. Superfluous
    files have been eliminated from tarball. <span
    id="LatestNews.line-251" class="anchor"></span>
-   The spec file no longer requires libsysfs &lt; 2 as OpenHPI now
    builds against libsysfs 2 fine. <span id="LatestNews.line-252"
    class="anchor"></span>
-   Fixed the gcc version check, which now works on gcc 4.2. <span
    id="LatestNews.line-253" class="anchor"></span>

Special thanks to crinaldo for the Solaris patches. <span
id="LatestNews.line-254" class="anchor"></span><span
id="LatestNews.line-255" class="anchor"></span>

For a complete list of 2.10.1 bug fixes and features, take a look at the
[changelog](Changelogs/2.10.1.md). <span
id="LatestNews.line-256" class="anchor"></span><span
id="LatestNews.line-257" class="anchor"></span>

##### PyOpenHPI 1.1 is released (09/28/07) 

<span id="LatestNews.line-258" class="anchor"></span>

The python module for OpenHPI has been formally released on Sourceforge!
After a long while of being in beta state and only available through the
subversion repository, now you can download a stable tarball to play
with. <span id="LatestNews.line-259" class="anchor"></span><span
id="LatestNews.line-260" class="anchor"></span>

In this first official release, the module now recognizes the following
new APIs: <span id="LatestNews.line-261" class="anchor"></span><span
id="LatestNews.line-262" class="anchor"></span>

-   saHpiResourceLoadIdGet <span id="LatestNews.line-263"
    class="anchor"></span>
-   saHpiResourceLoadIdSet <span id="LatestNews.line-264"
    class="anchor"></span>
-   saHpiGetIdByEntityPath <span id="LatestNews.line-265"
    class="anchor"></span>
-   saHpiGetChildEntityPath <span id="LatestNews.line-266"
    class="anchor"></span>
-   saHpiResourceFailedRemove <span id="LatestNews.line-267"
    class="anchor"></span>
-   saHpiEventLogCapabilitiesGet <span id="LatestNews.line-268"
    class="anchor"></span>
-   saHpiIdrFieldAddById <span id="LatestNews.line-269"
    class="anchor"></span>
-   saHpiIdrAreaAddById <span id="LatestNews.line-270"
    class="anchor"></span>
-   saHpiDimiInfoGet <span id="LatestNews.line-271"
    class="anchor"></span>
-   saHpiDimiTestInfoGet <span id="LatestNews.line-272"
    class="anchor"></span>
-   saHpiDimiTestReadinessGet <span id="LatestNews.line-273"
    class="anchor"></span>
-   saHpiDimiTestStart <span id="LatestNews.line-274"
    class="anchor"></span>
-   saHpiDimiTestCancel <span id="LatestNews.line-275"
    class="anchor"></span>
-   saHpiDimiTestStatusGet <span id="LatestNews.line-276"
    class="anchor"></span>
-   saHpiDimiTestResultsGet <span id="LatestNews.line-277"
    class="anchor"></span>
-   saHpiFumiSourceSet <span id="LatestNews.line-278"
    class="anchor"></span>
-   saHpiFumiSourceInfoValidateStart <span id="LatestNews.line-279"
    class="anchor"></span>
-   saHpiFumiSourceInfoGet <span id="LatestNews.line-280"
    class="anchor"></span>
-   saHpiFumiTargetInfoGet <span id="LatestNews.line-281"
    class="anchor"></span>
-   saHpiFumiBackupStart <span id="LatestNews.line-282"
    class="anchor"></span>
-   saHpiFumiBankBootOrderSet <span id="LatestNews.line-283"
    class="anchor"></span>
-   saHpiFumiBankCopyStart <span id="LatestNews.line-284"
    class="anchor"></span>
-   saHpiFumiInstallStart <span id="LatestNews.line-285"
    class="anchor"></span>
-   saHpiFumiUpgradeStatusGet <span id="LatestNews.line-286"
    class="anchor"></span>
-   saHpiFumiTargetVerifyStart <span id="LatestNews.line-287"
    class="anchor"></span>
-   saHpiFumiUpgradeCancel <span id="LatestNews.line-288"
    class="anchor"></span>
-   saHpiFumiRollback <span id="LatestNews.line-289"
    class="anchor"></span>
-   saHpiFumiActivate <span id="LatestNews.line-290"
    class="anchor"></span>

Plus all the corresponding new structures and definitions. <span
id="LatestNews.line-291" class="anchor"></span><span
id="LatestNews.line-292" class="anchor"></span>

##### OpenHPI 2.10.0 has been released (09/20/2007) 

<span id="LatestNews.line-293" class="anchor"></span>

2.10 is the latest stable release. It contains implementation and entry
points for all new APIs added in the B.02.01 specification. Also, all
APIs have been revised and updated according to minor return code
changes and additions in B.02.01. Following a complete list of the new
implemented APIs: <span id="LatestNews.line-294"
class="anchor"></span><span id="LatestNews.line-295"
class="anchor"></span>

-   saHpiResourceLoadIdGet <span id="LatestNews.line-296"
    class="anchor"></span>
-   saHpiResourceLoadIdSet <span id="LatestNews.line-297"
    class="anchor"></span>
-   saHpiGetIdByEntityPath <span id="LatestNews.line-298"
    class="anchor"></span>
-   saHpiGetChildEntityPath <span id="LatestNews.line-299"
    class="anchor"></span>
-   saHpiResourceFailedRemove <span id="LatestNews.line-300"
    class="anchor"></span>
-   saHpiEventLogCapabilitiesGet <span id="LatestNews.line-301"
    class="anchor"></span>
-   saHpiIdrFieldAddById <span id="LatestNews.line-302"
    class="anchor"></span>
-   saHpiIdrAreaAddById <span id="LatestNews.line-303"
    class="anchor"></span>
-   saHpiDimiInfoGet <span id="LatestNews.line-304"
    class="anchor"></span>
-   saHpiDimiTestInfoGet <span id="LatestNews.line-305"
    class="anchor"></span>
-   saHpiDimiTestReadinessGet <span id="LatestNews.line-306"
    class="anchor"></span>
-   saHpiDimiTestStart <span id="LatestNews.line-307"
    class="anchor"></span>
-   saHpiDimiTestCancel <span id="LatestNews.line-308"
    class="anchor"></span>
-   saHpiDimiTestStatusGet <span id="LatestNews.line-309"
    class="anchor"></span>
-   saHpiDimiTestResultsGet <span id="LatestNews.line-310"
    class="anchor"></span>
-   saHpiFumiSourceSet <span id="LatestNews.line-311"
    class="anchor"></span>
-   saHpiFumiSourceInfoValidateStart <span id="LatestNews.line-312"
    class="anchor"></span>
-   saHpiFumiSourceInfoGet <span id="LatestNews.line-313"
    class="anchor"></span>
-   saHpiFumiTargetInfoGet <span id="LatestNews.line-314"
    class="anchor"></span>
-   saHpiFumiBackupStart <span id="LatestNews.line-315"
    class="anchor"></span>
-   saHpiFumiBankBootOrderSet <span id="LatestNews.line-316"
    class="anchor"></span>
-   saHpiFumiBankCopyStart <span id="LatestNews.line-317"
    class="anchor"></span>
-   saHpiFumiInstallStart <span id="LatestNews.line-318"
    class="anchor"></span>
-   saHpiFumiUpgradeStatusGet <span id="LatestNews.line-319"
    class="anchor"></span>
-   saHpiFumiTargetVerifyStart <span id="LatestNews.line-320"
    class="anchor"></span>
-   saHpiFumiUpgradeCancel <span id="LatestNews.line-321"
    class="anchor"></span>
-   saHpiFumiRollback <span id="LatestNews.line-322"
    class="anchor"></span>
-   saHpiFumiActivate <span id="LatestNews.line-323"
    class="anchor"></span>

Other featured highlights incude: <span id="LatestNews.line-324"
class="anchor"></span><span id="LatestNews.line-325"
class="anchor"></span>

-   Basic support for MicroTCA in IPMIDirect plugin (psangouard) <span
    id="LatestNews.line-326" class="anchor"></span>
-   Compilation problems on Solaris were reported and fixed (crinaldo).
    <span id="LatestNews.line-327" class="anchor"></span>
-   Added recognition of a much larger set of manufacturer ids. <span
    id="LatestNews.line-328" class="anchor"></span>
-   Eliminated the need for plugins to implement/export get\_event.
    <span id="LatestNews.line-329" class="anchor"></span>
-   Sysfs plugin now builds against libsysfs 1.x and 2.x. <span
    id="LatestNews.line-330" class="anchor"></span>
-   FRU resource can now report resource type failed events in addition
    to hotswap (psangouard). <span id="LatestNews.line-331"
    class="anchor"></span>

Thanks again to everyone who reported bugs and patches! <span
id="LatestNews.line-332" class="anchor"></span><span
id="LatestNews.line-333" class="anchor"></span>

For a complete list of 2.10.0 bug fixes and features, take a look at the
[changelog](Changelogs/2.10.0.md). <span
id="LatestNews.line-334" class="anchor"></span><span
id="LatestNews.line-335" class="anchor"></span>

##### OpenHPI 2.9.3 has been released (08/27/2007) 

<span id="LatestNews.line-336" class="anchor"></span>

This release contains various bug fixes mostly from the community, plus
the start of the DIMI implementation. Following, some highlights: <span
id="LatestNews.line-337" class="anchor"></span><span
id="LatestNews.line-338" class="anchor"></span>

-   New APIs implemented in the last release are now callable in
    the daemon. <span id="LatestNews.line-339" class="anchor"></span>
-   Added saHpiDimiInfoGet() base implementation <span
    id="LatestNews.line-340" class="anchor"></span>
-   PICMG added to the list of known manufacturers recognized
    by oh\_decode\_manufacturerid() <span id="LatestNews.line-341"
    class="anchor"></span>
-   Created macro for calling plugin abi - This will help eliminate the
    redefining of abi functions before using them. <span
    id="LatestNews.line-342" class="anchor"></span>
-   Adjusted saHpiControlSet for handling INVALID\_REQUEST. New return
    code with B.02.01. <span id="LatestNews.line-343"
    class="anchor"></span>
-   Added fan analog control to simulator plugin - For testing hpifan
    <span id="LatestNews.line-344" class="anchor"></span>
   
Thanks to everyone who reported bugs and patches! <span
id="LatestNews.line-345" class="anchor"></span><span
id="LatestNews.line-346" class="anchor"></span>

For a complete list of 2.9.3 bug fixes and features, take a look at the
[changelog](Changelogs/2.9.3.md). <span
id="LatestNews.line-347" class="anchor"></span><span
id="LatestNews.line-348" class="anchor"></span>

##### OpenHPI 2.9.2 has been released (06/25/2007) 

<span id="LatestNews.line-349" class="anchor"></span>

This release is the first to contain and use the new HPI B.02.01 header
file. This header file is 100% backwards compatible with B.01.01. That
means you can download 2.9.2 and run with it without concern. This
release contains infrastructure implementation for the following new
APIs: <span id="LatestNews.line-350" class="anchor"></span><span
id="LatestNews.line-351" class="anchor"></span>

-   saHpiResourceLoadIdGet <span id="LatestNews.line-352"
    class="anchor"></span>
-   saHpiResourceLoadIdSet <span id="LatestNews.line-353"
    class="anchor"></span>
-   saHpiGetIdByEntityPath <span id="LatestNews.line-354"
    class="anchor"></span>
-   saHpiGetChildEntityPath <span id="LatestNews.line-355"
    class="anchor"></span>
-   saHpiResourceFailedRemove <span id="LatestNews.line-356"
    class="anchor"></span>
-   saHpiEventLogCapabilitiesGet <span id="LatestNews.line-357"
    class="anchor"></span>
-   saHpiIdrFieldAddById <span id="LatestNews.line-358"
    class="anchor"></span>
-   saHpiIdrAreaAddById <span id="LatestNews.line-359"
    class="anchor"></span>

Users can now configure the default domain itself by using domain id 0
in the configuration file in order to have control over which entity
paths can exist in the domain along with other domain capabilities.
<span id="LatestNews.line-360" class="anchor"></span><span
id="LatestNews.line-361" class="anchor"></span>

The new APIs implementation is still in beta and not available in daemon
mode. To build OpenHPI in non-daemon mode, use --disable-daemon during
configure. <span id="LatestNews.line-362" class="anchor"></span><span
id="LatestNews.line-363" class="anchor"></span>

For a complete list of 2.9.2 bug fixes and features, take a look at the
[changelog](Changelogs/2.9.2.md). <span
id="LatestNews.line-364" class="anchor"></span><span
id="LatestNews.line-365" class="anchor"></span>

##### OpenHPI 2.9.1 has been released (05/04/2007) 

<span id="LatestNews.line-366" class="anchor"></span>

OpenHPI 2.9.1 is a development release. Our focus is to use the 2.9.x
branch for implementing the HPI-B.02.01 specification relased earlier
this year. 2.9.1 brings with it the following things on the road to
achieve HPI-B.02.01 support: <span id="LatestNews.line-367"
class="anchor"></span><span id="LatestNews.line-368"
class="anchor"></span>

-   **Adpated return code changes from HPI B2** - The following APIs
    have either changed or clarified the return code logic in HPI B2:
    <span id="LatestNews.line-369" class="anchor"></span>

    -   saHpiEventGet <span id="LatestNews.line-370"
        class="anchor"></span>
    -   saHpiSensorThresholdsSet <span id="LatestNews.line-371"
        class="anchor"></span>
    -   saHpiControlSet <span id="LatestNews.line-372"
        class="anchor"></span>
    -   saHpiAnnunciatorGet <span id="LatestNews.line-373"
        class="anchor"></span>
    -   saHpiAnnunciatorAdd <span id="LatestNews.line-374"
        class="anchor"></span>
    -   saHpiEventLogEntryAdd <span id="LatestNews.line-375"
        class="anchor"></span>
    -   saHpiEventLogClear <span id="LatestNews.line-376"
        class="anchor"></span>
    -   saHpiEventLogTimeSet <span id="LatestNews.line-377"
        class="anchor"></span>
    -   saHpiEventLogStateSet <span id="LatestNews.line-378"
        class="anchor"></span>
    -   saHpiEventLogOverflowReset <span id="LatestNews.line-379"
        class="anchor"></span>
    -   saHpiEventAdd <span id="LatestNews.line-380"
        class="anchor"></span>
    -   saHpiAlarmGet <span id="LatestNews.line-381"
        class="anchor"></span>
    -   saHpiAlarmAdd <span id="LatestNews.line-382"
        class="anchor"></span>
    -   saHpiSensorEventMaskSet <span id="LatestNews.line-383"
        class="anchor"></span>
    -   saHpiControlGet <span id="LatestNews.line-384"
        class="anchor"></span>
    -   saHpiIdrFieldAdd <span id="LatestNews.line-385"
        class="anchor"></span>
    -   saHpiIdrFieldSet <span id="LatestNews.line-386"
        class="anchor"></span>
    -   saHpiAnnunciatorGet <span id="LatestNews.line-387"
        class="anchor"></span>
    -   saHpiAnnunciatorAdd <span id="LatestNews.line-388"
        class="anchor"></span>
    -   saHpiResourceResetStateSet <span id="LatestNews.line-389"
        class="anchor"></span>
-   **HPI Alarms** - Using the sensor enable change events also to
    detect proper alarm removals. <span id="LatestNews.line-390"
    class="anchor"></span>
-   **Added man page** documentation for the OpenHPI daemon (openhpid)
    <span id="LatestNews.line-391" class="anchor"></span>
-   **Blade``Center plugin** - Added resource firmware inventory data as
    Product Info <span id="LatestNews.line-392" class="anchor"></span>
-   **Fixed bug** to prevent discovery from running on a failed plugin.
    <span id="LatestNews.line-393" class="anchor"></span>

For a complete list of 2.9.1 bug fixes and features, take a look at the
[changelog](Changelogs/2.9.1.md). <span
id="LatestNews.line-394" class="anchor"></span><span
id="LatestNews.line-395" class="anchor"></span>

##### OpenHPI 2.8.1 has been released (02/26/2007) 

<span id="LatestNews.line-396" class="anchor"></span>

OpenHPI 2.8.1 is a patch release including about ten bugfixes of which
the most important are: <span id="LatestNews.line-397"
class="anchor"></span><span id="LatestNews.line-398"
class="anchor"></span>

-   **Daemon will no longer shutdown** if no plugins loaded at startup
    <span id="LatestNews.line-399" class="anchor"></span>
-   **Memory leak fixes** in the entity path utilities and blade center
    plugin <span id="LatestNews.line-400" class="anchor"></span>
-   **Adjustmenst for Daylight Savings** in the Blade``Center plugin
    <span id="LatestNews.line-401" class="anchor"></span>
-   **Fixes to the spec file** for older version of rpm and 64bit
    platforms <span id="LatestNews.line-402" class="anchor"></span>
-   Possible **array out of bounds condition** fixed in IPMI plugin
    <span id="LatestNews.line-403" class="anchor"></span>

For a complete list of 2.8.1 bugs, take a look at the
[changelog](Changelogs/2.8.1.md). <span
id="LatestNews.line-404" class="anchor"></span><span
id="LatestNews.line-405" class="anchor"></span>

##### OpenHPI 2.8.0 has been released (01/26/2007) 

<span id="LatestNews.line-406" class="anchor"></span>

2.8.0 is now the latest OpenHPI stable release. Following, the
highlights: <span id="LatestNews.line-407" class="anchor"></span><span
id="LatestNews.line-408" class="anchor"></span>

-   **Much improved rpm spec file** - 'make rpm' from the source tree
    won't require root access anymore. RPMs are created per plugin
    isolating specific plugin dependencies. Plugin rpms only created
    depending on configure enable/disable parameters. Tailored to be
    64bit platform friendly. <span id="LatestNews.line-409"
    class="anchor"></span>
-   **Blade``Center plugin improvement** - Better event log time
    handling <span id="LatestNews.line-410" class="anchor"></span>
-   **Documentation installation issues resolved** - README installation
    and sgml documentation problems have been fixed. <span
    id="LatestNews.line-411" class="anchor"></span>
-   **Conformance bugs fixed** - several HPI conformance bugs discussed
    on the mailing list were fixed. <span id="LatestNews.line-412"
    class="anchor"></span>
-   ...lots of other bug fixes <span id="LatestNews.line-413"
    class="anchor"></span>

For a complete list of 2.8.0 bugs and features, take a look at the
[changelog](Changelogs/2.8.0.md). <span
id="LatestNews.line-414" class="anchor"></span><span
id="LatestNews.bottom" class="anchor"></span>

</div>

<span id="line-25" class="anchor"></span><span id="bottom"
class="anchor"></span>

