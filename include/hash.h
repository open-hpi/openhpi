/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 *
 * See URL below for algorithm.
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 */

#ifndef HASH_H
#define HASH_H

#define LONG_BITS 32

#if LONG_BITS == 32
#define GOLDEN_RATIO_PRIME 0x9e370001UL
#else
#define GOLDEN_RATIO_PRIME 0x9e37fffffffc0001UL
#endif

static inline unsigned long hash_long(unsigned long val, int bits)
{
	return ((val*GOLDEN_RATIO_PRIME) >> (LONG_BITS-bits));
}

static inline unsigned long hash_ptr(void *ptr, int bits)
{
	return hash_long((unsigned long)ptr, bits);
}
 
#undef LONG_BITS
#endif//HASH_H
