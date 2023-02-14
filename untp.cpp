/*****************/
/* TurboDepacker */
/*****************/
/* (c) 1997 42Bastian Schick

   last changes:
    97/12/25  42BS  first version
    97/12/26  42BS  added comments

*/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MWPT       0x4d575054
//'MWPT'
#define TPWM       0x5450574d
 //'TPWM'

void error(char * s,char *s1);
uint32_t depack(uint8_t *d,uint8_t *out,uint32_t length);

uint32_t LoadFile(char *fn,uint8_t ** data);
uint32_t SaveFile(char *fn,uint8_t * data,uint32_t len,uint8_t *head,uint32_t headlen);
uint32_t ReverseEndian(uint32_t d);

uint32_t depack(uint8_t *in,uint8_t *out,uint32_t length)
{
  uint8_t * out0 = out;
  char packbyte;
  uint8_t counter;

  while ( length )
  {
    packbyte = (char)*in++;

    for ( counter = 8; counter && length ; packbyte <<= 1,--counter)
    {
      if ( packbyte < 0 )
      {
        uint8_t b = *in;
        uint8_t  count = (b & 0x0f)+3;
        uint16_t off = (b & 0xf0)<<4|*(in+1);
        uint8_t * ref = out - off;
        in += 2;

        length -= count;

        while ( count )
        {
          *out++ = *ref++;
          --count;
        }

      }
      else
      {
        *out++ = *in++;
        --length;
      }
    }
  }
  return (out - out0);
}

int main(int argc,char **argv)
{
  uint32_t inlen,outlen;
  uint8_t *inbuffer,*outbuffer;
  char help[256];

  if (argc == 1)
  {
    printf("usage : tdp file\n");
    return 0;
  }
  while(++argv,--argc)
  {

    if ( (uint32_t)(inlen = LoadFile(*argv,&inbuffer))<0 )
    {
      printf("Could not load %s !\n",*argv);
      exit(-1);
    }

    if ( *(uint32_t *)inbuffer == MWPT ){
      outlen = ReverseEndian(*(uint32_t *)(inbuffer+4));
    } else if ( *(uint32_t *)inbuffer == TPWM ){
      outlen = *(uint32_t *)(inbuffer+4);
    } else {
      free(inbuffer);
      printf("Wrong header !\n");
      exit(-1);
    }

    outbuffer = (uint8_t *) malloc(outlen + 0x8000UL);

    depack(inbuffer+8,outbuffer,outlen);

    char * ptr;
    if ( (ptr = strchr(*argv,'.')) )
       *ptr = 0;
    strcpy(help,*argv);
    strcat(help,".utp");


    SaveFile(help,outbuffer,outlen,0,0);
  //  outbuffer[outlen]=0;
  //  printf("%d\n%s",outlen,(char *)outbuffer);
    free(inbuffer);
    free(outbuffer);
  }
}
/*****************************************************************************/
uint32_t ReverseEndian(register uint32_t d)
{
    return  ((d << 24) |
            ((d & 0x0000ff00) << 8) |
            ((d & 0x00ff0000) >> 8) |
            (d >> 24));
}
