MCS := dmcs
MONO := mono

.PHONY: all openhpi_baselib example example_iterators

all: example example_iterators

openhpi_baselib:
	${MCS} @openhpi_baselib.rsp

example: openhpi_baselib
	${MCS} @example.rsp

example_iterators: openhpi_baselib 
	${MCS} @example_iterators.rsp

clean:
	rm -f example.exe example_iterators.exe openhpi_baselib.dll
