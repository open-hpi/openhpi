#include <config.h>
#include <openhpi.h>

#ifdef DEBUG
#define DEBUG_LEVEL 1024
#else
#define DEBUG_LEVEL 32
#endif

static int debug_level = DEBUG_LEVEL;

void set_debug_level(int level) 
{
        debug_level = level;
}

int get_debug_level(void)
{
        return debug_level;
}
