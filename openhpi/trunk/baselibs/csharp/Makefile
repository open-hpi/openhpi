MCS := /home/avpak/usr/mono/bin/dmcs
MONO := /home/avpak/usr/mono/bin/mono

.PHONY: all run example openhpi_baselib

all: run

openhpi_baselib:
	${MCS} @openhpi_baselib.rsp

example: openhpi_baselib
	${MCS} @example.rsp

run: example
	${MONO} example.exe

clean:
	rm -f example.exe openhpi_baselib.dll
