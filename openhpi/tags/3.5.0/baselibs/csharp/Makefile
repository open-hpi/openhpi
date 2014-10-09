MCS := dmcs
MONO := mono

.PHONY: all openhpi_baselib example example_iterators example_handlers

all: example example_iterators example_handlers

openhpi_baselib:
	${MCS} @openhpi_baselib.rsp

example: openhpi_baselib
	${MCS} @example.rsp

example_iterators: openhpi_baselib 
	${MCS} @example_iterators.rsp

example_handlers: openhpi_baselib 
	${MCS} @example_handlers.rsp

clean:
	rm -f example.exe example_iterators.exe example_handlers.exe openhpi_baselib.dll
