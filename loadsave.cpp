/* load/save helper */

#include <stdio.h>
#include <stdlib.h>

#ifndef uchar
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;
#endif
/* LoadFile opens a file,allocates memory and loads it completly */

long LoadFile(char *fn,uchar **addr);
long LoadFile(char *fn,uchar **addr)
{
  FILE *handle;
  long len;

  if ( (handle = fopen(fn,"rb"))==NULL)
    return -1;

  fseek(handle,0L,SEEK_END);
  len = ftell(handle);
  fseek(handle,0L,SEEK_SET);

  if ( (*addr = (uchar *)malloc(len+0x10200L))==NULL)
  {
    fclose(handle);
    return -1;
  }
  if ( len != (long)fread(*addr,sizeof(uchar),len,handle) )
  {
    fclose(handle);
    free(*addr);
    return -1;
  }
  fclose(handle);
  return len;
}

/* SaveFile */
long SaveFile(char *fn,uchar * addr,ulong len,uchar *head,ulong headlen);
long SaveFile(char *fn,uchar * addr,ulong len,uchar *head,ulong headlen)
{
  FILE * handle;

  if ( (handle = fopen(fn,"wb")) == NULL)
    return -1;

  if ( headlen )
    if ( headlen != fwrite(head,sizeof(uchar),headlen,handle) )
    {
      fclose(handle);
      return -1;
    }

  len = fwrite(addr,sizeof(uchar),len,handle);
  fclose(handle);

  return ( len );
}
