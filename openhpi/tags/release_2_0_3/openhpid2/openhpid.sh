#! /bin/bash
#
# openhpid.sh    Start/Stop the openhpi daemon.
#
# description: openhpid is standard UNIX program which uses the OpenHPI \
#              APIs and provides a standard internet server to access those \
#              APIs for client programs.
# processname: openhpid
# config: the standard openhpi conf file specified on the command line or the env.
# pidfile: /var/run/openhpid.pid

# Source function library.
. /etc/init.d/functions

# See how we were called.

prog="openhpid"

start() {
	echo -n $"Starting $prog: "
	daemon openhpid
	RETVAL=$?
	echo
	[ $RETVAL -eq 0 ] && touch /var/lock/subsys/openhpid
	return $RETVAL
}

stop() {
	echo -n $"Stopping $prog: "
	killproc openhpid
	RETVAL=$?
	echo
	[ $RETVAL -eq 0 ] && rm -f /var/lock/subsys/openhpid
	return $RETVAL
}	

rhstatus() {
	status openhpid
}	

restart() {
  	stop
	start
}	

# reload() {
# 	echo -n $"Reloading cron daemon configuration: "
# 	killproc crond -HUP
# 	RETVAL=$?
# 	echo
# 	return $RETVAL
# }	

reload() {
	echo -n $"Reload not supported. "
	echo
	return 0
}	

case "$1" in
  start)
  	start
	;;
  stop)
  	stop
	;;
  restart)
  	restart
	;;
  reload)
  	reload
	;;
  status)
  	rhstatus
	;;
  condrestart)
  	[ -f /var/lock/subsys/openhpid ] && restart || :
	;;
  *)
	echo $"Usage: $0 {start|stop|status|reload|restart|condrestart}"
	exit 1
esac
