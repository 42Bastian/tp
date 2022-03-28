#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define LITTLE_ENDIAN 0x1234
#define BIG_ENDIAN 0x3412
#define BYTE_ORDER LITTLE_ENDIAN
#else
#include <endian.h>
#endif

#ifndef uchar
  typedef unsigned char  uchar;
  typedef unsigned short ushort;
  typedef unsigned long  ulong;
#endif
