#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if defined(__WINNT__)  || defined(__MINGW32__)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#ifdef __APPLE__
#include <machine/endian.h>
#else
#include <endian.h>
#endif
#endif
