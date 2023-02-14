/* load/save helper */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "loadsave.h"

/* LoadFile opens a file,allocates memory and loads it completly */

long LoadFile(char *fn,uint8_t **addr)
{
  FILE *handle;
  long len;

  if ( (handle = fopen(fn,"rb"))==NULL)
    return -1;

  fseek(handle,0L,SEEK_END);
  len = ftell(handle);
  fseek(handle,0L,SEEK_SET);

  if ( (*addr = (uint8_t *)malloc(len+0x10200L))==NULL)
  {
    fclose(handle);
    return -1;
  }
  if ( len != (long)fread(*addr,sizeof(uint8_t),len,handle) )
  {
    fclose(handle);
    free(*addr);
    return -1;
  }
  fclose(handle);
  return len;
}

/* SaveFile */
long SaveFile(char *fn,uint8_t * addr,uint32_t len,uint8_t *head,uint32_t headlen)
{
  FILE * handle;

  if ( (handle = fopen(fn,"wb")) == NULL)
    return -1;

  if ( headlen )
    if ( headlen != fwrite(head,sizeof(uint8_t),headlen,handle) )
    {
      fclose(handle);
      return -1;
    }

  len = fwrite(addr,sizeof(uint8_t),len,handle);
  fclose(handle);

  return ( len );
}
