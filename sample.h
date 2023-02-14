/* sample.h */

#ifndef __SAMPLE_H__

#define __SAMPLE_H__

typedef struct SCommonHead
  {
    uint32_t length;
    uint32_t frq;
    uint16_t stereo;
    uint8_t * data;
  } tCommonHead;

void Histogram(tCommonHead &sample, uint16_t histo[], uint8_t &min, uint8_t &max);
void ShowHistogram(tCommonHead &sample);
void ChangeVolume(tCommonHead &sample, uint16_t ratio);
void Change2Signed(tCommonHead &sample);
uint32_t SamplePacker(tCommonHead &sample, uint8_t ** packed);

#endif
