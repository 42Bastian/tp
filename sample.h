/* sample.h */

#ifndef __SAMPLE_H__

#define __SAMPLE_H__

typedef struct SCommonHead
  {
    long length;
    long frq;
    short stereo;
    uchar * data;
  } tCommonHead;

void Histogram(tCommonHead &sample, ushort histo[], uchar &min, uchar &max);
void ShowHistogram(tCommonHead &sample);
void ChangeVolume(tCommonHead &sample, ushort ratio);
void Change2Signed(tCommonHead &sample);
long SamplePacker(tCommonHead &sample, uchar ** packed);

#endif
