/*********************************
 * common sample helper routines *
 *********************************/

/*
  42BS      42Bastian Schick (elw5basc@gp.fht-esslingen.de)

  history :
  YY/MM/DD
  98/01/06-     42BS    created


*/

#include "global.h"
#include "sample.h"
#include <stdlib.h>

void Histogram(tCommonHead &smp,uint16_t histo[], uint8_t &min, uint8_t &max)
{
  uint8_t _min = 255;
  uint8_t _max = 0;
  uint8_t c;
  uint8_t * data = smp.data;
  long len = smp.length;

  memset(histo,0,256*sizeof(uint16_t));

  while ( len-- )
  {
    ++histo[ c = *data++ ];
    if ( c > _max ) _max = c;
    if ( c < _min ) _min = c;
  }
  min = _min;
  max = _max;
}

void ChangeVolume(tCommonHead &smp, uint16_t ratio)
{
  uint8_t * data = smp.data;
  long len = smp.length;

  while ( len-- )
  {
    uint16_t val = (*data)*ratio/100;
    *data++ = val < 256 ? val : 255;
  }
}

void Change2Signed(tCommonHead &smp)
{
  uint8_t * data = smp.data;
  long len = smp.length;

  while ( len-- )
    *data++ ^= 0x80;
}

int8_t power[]={ -128,-64,-32,-16,-8,-4,-2,-1,0,1,2,4,8,16,32,64};
int16_t times[]={ 128,  64, 32, 16, 8, 4, 2, 1,1,1,2,4,8,16,32,64};
uint32_t SamplePacker(tCommonHead &sample,uint8_t **data)
{
  int8_t last;
  int16_t delta;
  uint8_t nibble1,nibble2;
  uint8_t *ptr;
  uint8_t *unpacked = sample.data;
  uint32_t len = ((sample.length + 1) & 0xffffffeUL)/2;

  if ( (*data = ptr = (uint8_t *)malloc(len)) == 0)
    return (-1);

  uint8_t nibble_encode[256*2];
  uint8_t * pnibble = nibble_encode;
  for(int16_t i = 0; i < 16; ++i)
    for(int16_t j = times[i]; j ; --j)
      *pnibble++ = i;

  memset(pnibble,0,128);

  if ( sample.stereo )
  {
    // not yet
  }
  else
  {
    last = 0;
    long len2 = len;
    while ( len2-- )
    {
      delta = ((int16_t)*(int8_t *)unpacked++) - (int16_t)last;
      nibble1 = nibble_encode[delta+255];
      last += (int16_t)power[nibble1];

      delta = ((int16_t)*(int8_t *)unpacked++) - (int16_t)last;
      nibble2 = nibble_encode[delta+255];
      last += (int16_t)power[nibble2];

      *ptr++ = (nibble1<<4)|nibble2;
    }
  }
  return len;
}

void SampleDePacker(tCommonHead &sample,uint8_t *data)
{
  uint8_t *ptr = sample.data;
  long len = (sample.length + 1)>>1;
  int8_t tmp =0;
  uint8_t nibble;

  while ( len -- ) {
    nibble = *data++;
    tmp += power[nibble>>4];
    *ptr++ = tmp;
    tmp += power[nibble & 0xf];
    *ptr++ =(uint8_t) tmp;
  }
}
/****************************************/

void ShowHistogram(tCommonHead &sample)
{
  uint16_t histo[256];
  uint16_t *phisto = histo;
  uint8_t min,max;

  Histogram( sample, histo, min, max);

  printf("\n Min : %3d  Max : %3d \n",min,max);
  for ( int16_t i = 16 ; i ; --i)
  {
    for ( int16_t j = 16 ; j ; --j)
    {
      printf("%4d ",*phisto++);
    }
//    printf("\n");
  }
}
