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

#ifndef uchar
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;
#endif

#define MWPT       0x4d575054
//'MWPT'
#define TPWM       0x5450574d
 //'TPWM'

void error(char * s,char *s1);
ulong depack(uchar *d,uchar *out,ulong length);

ulong LoadFile(char *fn,uchar ** data);
ulong SaveFile(char *fn,uchar * data,ulong len,uchar *head,ulong headlen);
ulong ReverseEndian(ulong d);

ulong depack(uchar *in,uchar *out,ulong length)
{
  uchar * out0 = out;
  char packbyte;
  uchar counter;
  
  while ( length )
  {

    packbyte = (char)*in++;
    
    for ( counter = 8; counter && length ; packbyte <<= 1,--counter)
    {
      if ( packbyte < 0 )
      {
        uchar  count = (*in & 0x0f)+3;
        uchar * ref = out - ( (ushort)(*in & 0xf0) << 4 | *(in+1) );
        in +=2;
        
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
  ulong inlen,outlen;
  uchar *inbuffer,*outbuffer;
  char help[256];
  
  if (argc == 1)
  {
    printf("usage : tdp file\n");
    return 0;
  }
  while(++argv,--argc)
  {
  
    if ( (long)(inlen = LoadFile(*argv,&inbuffer))<0 )
    {
      printf("Could not load %s !\n",*argv);
      exit(-1);
    }

    if ( *(long *)inbuffer == MWPT ){
      outlen = ReverseEndian(*(long *)(inbuffer+4));
    } else if ( *(long *)inbuffer == TPWM ){
      outlen = *(long *)(inbuffer+4);
    } else {
      free(inbuffer);
      printf("Wrong header !\n");
      exit(-1);
    }

    outbuffer = (uchar *) malloc(outlen + 0x8000UL);

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
ulong ReverseEndian(register ulong d)
{
    return  ((d << 24) |
            ((d & 0x0000ff00) << 8) |
            ((d & 0x00ff0000) >> 8) |
            (d >> 24));
}
