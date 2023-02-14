/************************
 * WAV to LNX converter *
 * with sample.packer   *
 ************************/

#define VER "1.00"

#include "global.h"
#include "sample.h"
#include "loadsave.h"

void SampleDePacker(tCommonHead &sample,uint8_t *data);

struct SWavSmpHead
{
  long Magic;       // 'fmt '
  long chunklen;    // without chunk-header : here 16
  int16_t weisnich;
  int16_t stereo;     // 1 = Mono 2 = Stereo
  uint32_t playfrq;
  uint32_t smplfrq;
  int16_t weisnich2;
  int16_t bps;        // 8 or 16
};

struct SLnxSmpHead
{
  uint8_t stereo;     // != 0 stereo
  uint8_t length[2];  // NOTE : big endian
  uint8_t reload;     // Timer reload : 1MHz/sample_frq - 1
  uint8_t unpacked;   // != 1 unpacked data
};

uint16_t NoWave(uint8_t *in, long len);
void SaveLsf(char *name,tCommonHead &sample,bool packed);

/* some helpers */
void error(const char * s,const char *s1);
uint32_t ReverseEndian(uint32_t d);

//
// do the nasty little/big-endian thing
// (Is there any >16bit CPU besides x86-based that use LE ??)
//
#if BYTE_ORDER == LITTLE_ENDIAN
#  define LE2BE(a)   ReverseEndian(a)
#  define BE2LE(a)   (a)
#else
#  define LE2BE(a)   (a)
#  define B22LE(a)   ReverseEndian(a)
#endif

uint16_t NoWave(uint8_t *in, long len)
{

  if ( LE2BE(*(long *)in) != 'RIFF' )
    return 1;

  if ( BE2LE(*(long *)(in+4))+8 != len )
    return 1;

  if ( LE2BE(*(long *)(in+8)) != 'WAVE'  )
    return 1;

  return 0;
}


tCommonHead WorkSample;

void WaveToCommon(uint8_t *in,long len)
{
  long skip;
  uint8_t * help = in + 12; // pointer behind 'WAVE'

  while ( LE2BE(*(long *)help) != 'data')
    help += 8 + BE2LE(*(long *)(help+4));

  long sample_len = BE2LE(*(long *)(help +4));
  uint8_t *sample_data = help + 8;

  help = in + 12;

  while ( LE2BE(*(long *)help) != 'fmt ')
    help += 8 + BE2LE(*(long *)(help+4));

  struct SWavSmpHead * wavhead = (struct SWavSmpHead *)help;

  WorkSample.length = sample_len;
  WorkSample.frq = BE2LE(wavhead->smplfrq);
  WorkSample.data = sample_data;
  WorkSample.stereo = (wavhead->stereo == 0x0200) || (wavhead->stereo == 2);
}

void SaveLsf(char *name,tCommonHead &sample,bool packed)
{
  struct SLnxSmpHead head;
  uint8_t * d;
  uint32_t l;

  Change2Signed(sample);

  head.stereo = (uint8_t)sample.stereo;
  head.reload = (uint8_t) (1e6/sample.frq)-1;

  if ( packed ) {
    if ( (l = SamplePacker(sample,&d)) < 0 ){
      return;
    }

    tCommonHead s2;
    s2.data = (uint8_t *)malloc( s2.length = (sample.length+1)>>1);

    SampleDePacker(s2,d);
    l = sample.length;
    uint8_t *ptr = s2.data;
    uint8_t *ptr2= d = sample.data;
//    while ( l--)
//    {
//      *ptr2++ -= *ptr++;
//    }
//    l = sample.length;

    d = s2.data;

    head.length[0] = (l>>8)& 0xff;
    head.length[1] = (l & 0xff);
    head.unpacked = 1;
    SaveFile(name,d,l,(uint8_t *)&head,sizeof(struct SLnxSmpHead));
  } else {
    head.length[0] = (sample.length>>8)& 0xff;
    head.length[1] = (sample.length & 0xff);
    head.unpacked = 1;
    SaveFile(name,sample.data,sample.length,(uint8_t *)&head,sizeof(struct SLnxSmpHead));
  }
}


int main(int argc,char **argv)
{
  uint8_t * in,*out;
  long in_len;
  char * in_name = 0;
  char * out_name = 0;
  bool packed = false;
  bool histo  = false;
  int16_t ratio = 0;
  int offset  = 0;

  if ( argc == 1 ){
    error("Usage : wavb2lnx [-h] [-v percentage] [-o out] in","");
  }

  --argc;++argv;

  while ( argc )
  {
    if ( ! strcmp(*argv,"-p") ) {
      --argc; ++argv;
      packed = true;
      continue;
    }
    if ( ! strcmp(*argv,"-h") ) {
      --argc; ++argv;
      histo = true;
      continue;
    }
    if ( ! strcmp(*argv,"-off") ) {
      --argc; ++argv;
      if (argc)
      {
        offset = atoi(*argv);
        --argc; ++argv;
      } else {
        error("Offset value missing !","");
      }
      continue;
    }

    if ( ! strcmp(*argv,"-o") ) {
      --argc; ++argv;
      if ( argc )   {
        out_name = *argv;
        --argc; ++argv;
        continue;
      }
      error("Missing output file after -o !\nAborting ...","");
    }
    if ( ! strcmp(*argv,"-v") ) {
      --argc;++argv;
      if ( argc )  {
        ratio = atoi(*argv);
        --argc; ++argv;
        continue;
      } else {
        error("Missing ratio for -v !\n","");
      }
    }

    if ( **argv == '-' ){
      error("Unknown option.\nAborting ...","");
    }

    if ( in_name ) {
      error("Multiple input-files.\nAborting ...","");
    }

    in_name = *argv;
    --argc; ++argv;
  }

  if ( (in_len = LoadFile(in_name,&in)) < 0 ){
    error("Couldn't load %s !",in_name);
  }

  if ( NoWave(in+offset, in_len-offset) ) {
    free(in);
    error("This is no WAV-file !","");
  }

  WaveToCommon(in+offset,in_len-offset);
  printf("Sample-length:%d\n",WorkSample.length);
  printf("Sample-Frq:%d\n",WorkSample.frq);

//  ShowHistogram(WorkSample);
//  ChangeVolume(WorkSample,50);
  if ( histo ){
    ShowHistogram(WorkSample);
  }

  if ( ratio ){
    ChangeVolume(WorkSample,ratio);
  }

  if ( histo ){
    ShowHistogram(WorkSample);
  }

  if ( out_name ){
    SaveLsf(out_name,WorkSample,packed);
  }

  free(in);
}

uint32_t ReverseEndian(register uint32_t d)
{
  return  ((d << 24) |
           ((d & 0x0000ff00) << 8) |
           ((d & 0x00ff0000) >> 8) |
           (d >> 24));
}

void error(const char *s,const char *s1)
{
  printf("wav2lsf " VER " " __DATE__ "\n");
  printf(s,s1);
  printf("\n");
  exit(-1);
}
