from openhpi_baselib.Ohpi import *


#**********************************************************
# OHPI Utility Functions: Iterators
#**********************************************************

#**********************************************************
# Iterate over Handlers
# Provides handler id
#**********************************************************
def HandlerIds( sid ):
    hid = SAHPI_FIRST_ENTRY
    while hid != SAHPI_LAST_ENTRY:
        ( rv, hid ) = oHpiHandlerGetNext( sid, hid )
        if rv != SA_OK:
            break
        yield hid

