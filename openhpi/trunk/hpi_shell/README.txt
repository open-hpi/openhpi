
		hpi_cmd

	Command interface to an Openhpi system.

Synopsis:

	hpi_cmd [-c <cfgfile>] [-e] [-f <file name>]

Description:

	The hpi_cmd allows show and control Openhpi resources. It runs in two modes:
		- packet;
		- dialog;
	Before executing commands hpi_cmd opens HPI session for a default domain and
	runs discovery in it. In the packet mode commands are read from the input file
	passed with -f option or as a parameter for the run command. In the dialog
	mode commands are read from the standard input (stdin).
	Lines beginning with a # are comments. Symbol ; is a delimiter in a command
	sequance.

Options:

	-c <cfgfile>
		<cfgfile> is used as a Openhpi configuration file.

	-e
		Event is enable and show events in the short format, subscribe for
		events, before discovery call.

	-f <file name>
		Execute a command file <file name>.


Commands:

	ann <rptId> <annNum>
		Annunciator block commands. This block contains show and set commands for
		annunciator resource <rptId>/<annNum>.
	    show - show annunciator.
	    show <EntryId> - show announcement by EntryId.
	    list - show announcement list.
	    modeget - show annunciator mode.
	    modeset <mode> - set annunciator mode.
	    add <values> - add new announcement.
	    delete <entryId> - delete announcement by EntryId.
	    acknow <EntryId> - acknowledge a single announcement.
	    acknow all <Severity> - acknowledge a group of announcements.

	clearevtlog [<rptId>]
		Erase the contents of the Event Log for the resource <rptId>. If <rptId> 
		is omitted, a current domain Event Log is erased.

	ctrl <rptId> <ctrlId>
		Controls block commands. This block contains show and set commands for
		control resource <rptId>/<ctrlId>.
	    show - show control.
	    state - show the control state.
	    setstate [<value>] - set control state. <value> depends on control type:
	    		for digital: <value> = on | off | pulseon | pulseoff
	    		for discrete: <value> = integer value
	    		for analog: <value> = integer value
	    		for stream: <value> = <repeat> string, where <repeat> = yes | no
	    		for text: <value> = <line#> string, where <line#> = integer
	    		for oem: <value> = <Mid> string, where <Mid> = integer

	dat
		Show domain alarm table list.

	debug [on | off]
		Sets and unsets OPENHPI_DEBUG environment. If a parameter is omitted,
		a current value OPENHPI_DEBUG is showed.

	domain [<domainId>]
		Set current domain by <domainId>. Session is opened for this domain and
		discovery is called (if necessary). If parameter is omitted, command
		shows a list of the domains.

	domaininfo
		Show current domain info.

	dscv
		Discover information about resources for the current domain.

	echo [<string>]
		Print <string> to the stdout. if <string> is omitted, a empty line is
		printed.

	event [enable | disable | full | short]
		Set and unset show events enable. <value> is enable (show enable), disable
		(show disable), full (show enable in full format) or short (show enable in
		short format). if <value> is omitted, a current value of the show event is
		printed.

	evtlogreset [<resourceId>]
		Reset the OverflowFlag in the Event Log for passed resource. If parameter
		is omitted, reset the OverflowFlag for current domain.

	evtlogstate [<resourceId>]
		Show Event Log state for passed resource. If parameter is omitted, show the
		current domain Event Log state.

	evtlogtime [<resourceId>]
		Show Event Log's clock. If parameter is omitted, show the current domain
		Event Log's clock.

	help [<command>]
		Help information about passed command. If <command> is omitted, a command
		list is printed.

	hs <resourceId>
		Hot swap block commands.
	    action <type> - set action process. <type> - insert|extract
	    active - set active state.
	    gettimeout <type> - show timeout. <type> - insert|extract
	    inactive - set inactive state.
	    ind <action> - set and show indicator state. <action> - get|on|off
	    policycancel - set default policy.
	    settimeout <type> <value> - set timeout. <type> - insert|extract
	    state - show hot swap state.

	inv <rptId> <IdrId>
		Inventory block commands. This block contains show and set commands for
		inventory resource <rptId>/<IdrId>.
	    show - show inventory.
	    addarea <type> - add area to the inventory. <type> - area type:
	    		inter | chass | board | prod | oem
	    delarea <num> - delete area with Area Id <num> from the inventory.
	    addfield <AreaID> <type> <string> - add field to the area <AreaId>.
	    		<type> - chass | time | manuf | prodname | prodver | snum |
	    			 pnum | file | tag | custom
	    		<string> - field value.
	    setfield <AreaID> <FieldId> <type> <string> - set new value for field <FieldID>
	    		from area <AreaId>.
	    delfield <AreaID> <FieldId> - delete field <FieldID> from area <AreaId>.

	lsres
		Show resources list.

	lsensor
		Show sensors list.

	parmctrl <resourceId> default | save | restore
		Save and restore parameters associated with a specific resource.

	power <resourceId> [on | off | cycle]
		Set power state for resource <resourceId>. If second parameter is omitted,
		show current power state.

	quit
		Quit command block or end hpi_cmd program.

	reset <resourceId> [cold | warm | assert | deassert]
		Reset resource <resourceId>.

	rdr <resourceId> <type> <rdrnum>
		(see showrdr)

	rpt <resourceId>
		(see showrpt)

	run <path>
		Run command file <path>.

	sen <rptId> <SensorId>
		Sensor block commands. This block contains show and set commands for
		sensor <rptId>/<IdrId>.
	    show - show sensor current state.
	    enable - set sensor enable.
	    disable - set sensor disable.
	    evtenb - set sensor event enable.
	    evtdis - set sensor event disable.
	    maskadd <Assert mask> <Deassert mask> - set the corresponding bits in the
	    	sensor's assert and deassert event masks.
	    maskrm <Assert mask> <Deassert mask> - remove the corresponding bits in the
	    	sensor's assert and deassert event masks.
	    setthres <type> <value> [<type> <value> ...] - set sensor threshold.
	        <type> = li (Lower Minor), la (Lower Major), lc (Lower Critical),
	        	 ui (Upper Minor), ua (Upper Major), uc (Upper Critical),
	        	 ph (Positive Hysteresis), nh (Negative Hysteresis)
	        <value> - new sensor threshold value.
	        for example:
	        	setthres li 300 ui 303 la 290

	setsever <resourceId>   crit | maj | min | inf | ok | debug | all
		Set the severity level for resource <resourceId>.

	settag <resourceId> <new tag>
		Set the tag <new tag> for resource <resourceId>.

	showevtlog <resourceId>
		Show Event Log for resource <resourceId>. If parameter is omitted, show the
		current domain Event Log.

	showinv <resourceId>
		Show all inventory data for resource <resourceId>.

	showrdr <resourceId> <type> <rdrnum>
		Show Resource Data Record <rdrnum> with type <type> for resource
		<resourceId>. <type> = c - control, s - sensor, i - inventory
		w - watchdog, a - annunciator, all - all rdrs.

	showrpt <resourceId>
		Show resource information.

	wtdget <resourceId> <watchdogNum>
		Show current watchdog timer settings and configuration.

	wtdset <resourceId> <watchdogNum> <values>
		Set watchdog timer configuration.
	    <values> = <Log> <Running> <TimerUse> <TimerAction> <PretimerInterrupt>
	    	       <TimeOut> <Flags> <InitialCount>

	    <Log> = 0 | 1
	    		- events will be generated;
	    
	    <Running> = 0 | 1
	    		- Watchdog should be stopped before updating;
	    
	    <TimerUse> = none | bios_frb2 | bios_post | os_load | sms_os | oem
	    		- current use of the timer;
	    
	    <TimerAction> = no | reset | pwr_down | pwr_cycle | int
	    		- what action will be taken when the Watchdog times out;
	    
	    <PretimerInterrupt> = no | smi | nmi | mess | oem
	    		- action for TimeOut;
	    
	    <TimeOut> = integer (milliseconds);
	    
	    <Flags> = "bios_frb2 | bios_post | os_load | sms_os | oem"
	    		- Set of five bit flags corresponding to the five TimerUse values;
	    
	    <InitialCount> = integer (milliseconds);

	wtdreset <resourceId> <watchdogNum>
		Reset watchdog timer.



Redirection output.

	Output of the command can be redirect to the local file:
		command > file		- redirect standard output to the passed file.
		command >> file		- add standard output to the passed file.

Packet mode.

	In this mode commands are read from the command file. All commands described
	before are available in the packet mode. If the executed command is fail the
	packet mode is canceled and switches to the dialog mode.


Hpi_cmd is a client of the Openhpi daemon.

	Hpi_cmd can run as a client of the Openhpi daemon. For this purpose you must
	configure Openhpi with flag --enable-shell_client :

		./configure --enable-shell_client

	In this case openhpid and hpi_cmp are built.

