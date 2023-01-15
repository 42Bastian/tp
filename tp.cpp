/* ported from 68k source by 42Bastian Schick
   (c) 68k version W.Mayer
   (c) C version 1997 42Bastian Schick

   changes :

   97/12/23..24 42BS  first version
   97/12/26     42BS  build in [1] (see below)
   added comments
   recognizes Lynx files and creates special header
   98/01/05     42BS  Added BE/LE code for portabability reasons
   07/01/04     42BS  Fixed exception bug if only one file is packed
*/

#define VER "1.04"

#include "global.h"

// [1] Some (modern) CPUs allow word-access on odd addresses.
//     If yours does not, un-comment the following #define !
//#define  WORD_ALIGN_NEEDED

void error(const char * s,const char *s1);
void CreatTable(ushort * tab,uchar *ptr);
long Packer(uchar *in,long length,ushort * table,ushort * addrtab,bool jump);
void PrintPercentage(long packed,long unpacked,long done);
ulong ReverseEndian(ulong d);

//
// do the nasty little/big-endian thing
// (Is there any >16bit CPU besides x86-based that use LE ??)
//
#if BYTE_ORDER == LITTLE_ENDIAN
#  define BLL_MAGIC  0x0880
#  define BLL_PACKED 0x0980
#  define TPWM       0x4d575054
//'MWPT'
#  define LE2BE(a)   ReverseEndian(a)
#else
#  define BLL_MAGIC  0x8008
#  define BLL_PACKED 0x8009
#  define TPWM       0x5450574d
//'TPWM'
#  define LE2BE(a)   (a)
#endif

/* loadsave.cc */
long LoadFile(char *fn,uchar ** data);
long SaveFile(char *fn,uchar * data,ulong len,uchar *head,ulong headlen);

/* here is the beef ... */
void CreatTable(register ushort * tab,uchar *ptr)
{
  int i;
  memset((char *)tab,0,0x10001*sizeof(ushort));  // clear count-table

  ++tab;
  for (i = 0; i < 0x8000 ; ++i) {
#ifdef WORD_ALIGN_NEEDED
    ++tab[(ushort)(*ptr<<8)|*++ptr];
#else
    ++tab[*(ushort *)ptr]; ++ptr; //[1]
#endif
  }
  --tab;

  for (i = 0; i <= 0x10000L; ++i){
    tab[i+1] += tab[i];
  }
}

long Packer(uchar *in0,
            long length,
            unsigned short * table,
            unsigned short * addrtab,
            bool jumptable)
{
  uchar * in,* in_ptr;
  uchar * end_notpacked;
  uchar * out_ptr;
  uchar * ppackbyte;
  uchar counter;
  uchar * ptable = 0;             // force building of a new table
  ushort offset,offset1,offset2;
  int i;

  in = in_ptr = in0;
  end_notpacked = out_ptr = in + length;

  ptable = in_ptr-0x10000; /* force new table */

  PrintPercentage(0,length,0);

  while ( end_notpacked != in_ptr )
  {
    ppackbyte = out_ptr++;   // save address for pack-byte

    PrintPercentage(out_ptr-end_notpacked,length,in_ptr-in);

    for ( *ppackbyte = 0,counter = 8;
          counter && end_notpacked != in_ptr ;
          --counter )
    {
      *ppackbyte <<= 1;

      if ( end_notpacked - in_ptr > 3)  {
        uchar * save_in = in_ptr;
//
// setup new hash(?) table
//
        if ( in_ptr - ptable >= 0x8000L ) {
          ptable = in_ptr - 0xfff;
          if ( ptable < in ){
            ptable = in;
          }

          CreatTable(table,ptable);

          uchar * ptr = ptable;

          for( i = 0; i < 0x8000U; ++i) {
#ifdef WORD_ALIGN_NEEDED
            offset1 = (*ptr<<8)| *++ptr; // fetch word , make it offset
#else
            offset1 = *(ushort *)ptr; ++ptr; // [1]
#endif
            addrtab[table[offset1]]=i;
            ++table[offset1];
          }
          CreatTable(table,ptable);
        }
        if ( in_ptr - 0xfff < in )          // minimal look-back address
          offset = in - ptable;
        else
          offset = in_ptr - 0xfff - ptable;

#ifdef WORD_ALIGN_NEEDED
        offset1 = (*in_ptr<<8)|*(in_ptr+1); // fetch current word
#else
        offset1 = *(ushort *)in_ptr; // [1]
#endif
        for( offset2 = table[offset1];
             addrtab[offset2] < offset;
             ++offset2)
        {
          /* empty */
        }
        table[offset1]= offset2;  // next time start searching here !

        ptable += 2;

        in_ptr += 2;

        uchar d3 = *in_ptr++;
        uchar bytecount,
          max_count = 0;
        uchar * sqz_ptr;

        do{
          uchar * ptr;
//
// we want at least a 3 byte sequence
//
          do{
            ptr = ptable + addrtab[offset2++];
          }while ( *ptr++ != d3 && ptr < end_notpacked);

          if ( ptr == in_ptr ) break; // same byte ?

          bytecount = 3;
          for( int max = 0; max<15 && (*ptr == in_ptr[max]) ; ++max)
          {
            ++bytecount;
            ++ptr;
          }

          if ( bytecount > max_count ) // find best sequence
          {
            max_count = bytecount;
            sqz_ptr = ptr;
          }

        }while ( bytecount != 18 );   // 18 , then no better possible

        ptable -= 2;

        in_ptr = save_in;

        if ( max_count )
        {
          *ppackbyte |= 1;            // set flag

          sqz_ptr -= max_count;
//
// pack only till end of data
//
          if ( max_count > end_notpacked - in_ptr )
            max_count = end_notpacked - in_ptr;

          offset = in_ptr - sqz_ptr;  // get offset (but positive !)

          in_ptr += max_count;        // update unpacked pointer

          max_count -= 3;             // count of 3 implied

          if ( jumptable ){

            // If we use a jumptable, we need an offset rather
            // than a counter.
            *out_ptr++ = (15-max_count) | ((offset >> 4) & 0xf0);
          } else {
            *out_ptr++ = max_count | ((offset >> 4) & 0xf0);
          }
          *out_ptr++ = (uchar) offset & 0xff;
          continue;
        }
      }
      *out_ptr++ = *in_ptr++; // store byte
    }// for (counter = ...
#if 1
//
// overwrite already packed data to save space
//
    uchar * ptable2 = in_ptr - 0x1000;
    long i;

    if ( (long long)ptable2 & 1) ++ptable2;

    if ( ptable2 > in && (i = ptable2 - in0) > 0x8000L )
    {
      memcpy((char *)in0,(const char *)ptable2,out_ptr-ptable2);
      in   -= i;
      end_notpacked -= i;
      in_ptr  -= i;
      out_ptr -= i;
      ptable  -= i;
    }
#endif
//
// check if data is un-packable
//
    if ((long)(out_ptr - in0) > length + 0x10000L){
      printf("Error\n");
      return 0;
    }
  }

  if ( counter != 8 )
    *ppackbyte <<= counter; // adjust packbyte
  else
    --out_ptr;              // skip last (unused) packbyte

  memcpy((char *)in0,(const char *)end_notpacked,out_ptr-end_notpacked);
//->  PrintPercentage(out_ptr-end_notpacked,length,in_ptr-in);
  printf("\n");
  return (out_ptr-end_notpacked);
}

void CreatInvTable(ushort * tab,uchar *ptr)
{
  int i;
  memset((char *)tab,0,0x10001*sizeof(ushort));  // clear count-table

  ++tab;
  for (i = 0; i < 0x8000 ; ++i) {
    ++tab[(ushort)(*ptr<<8)|*--ptr];
  }
  --tab;

  for (i = 0; i <= 0x10000L; ++i){
    tab[i+1] += tab[i];
  }
}

long InvPacker(uchar *in0,
               long length,
               unsigned short * table,
               unsigned short * addrtab,
               bool jumptable)
{
  uchar * in,* in_ptr;
  uchar * end_notpacked;
  uchar * out_ptr;
  uchar * ppackbyte;
  uchar counter;
  uchar * ptable = 0;             // force building of a new table
  ushort offset,offset1,offset2;
  int i;

  out_ptr = in0 + length;

  end_notpacked = in0;

  in = in_ptr = out_ptr;

  ptable = in_ptr+0x10000; /* force new table */

  PrintPercentage(0,length,0);

  while ( end_notpacked != in_ptr ) {
    ppackbyte = out_ptr++;   // save address for pack-byte

    PrintPercentage(out_ptr-end_notpacked,length,in_ptr-in);

    for ( *ppackbyte = 0,counter = 8;
          counter && end_notpacked != in_ptr ;
          --counter )
    {
      *ppackbyte <<= 1;

      if ( in_ptr - end_notpacked > 3)   {
        uchar * save_in = in_ptr;
        uchar * ptr;
//
// setup new hash(?) table
//
        if ( ptable - in_ptr <= -0x8000L )  {
          ptable = in_ptr + 0xfff;
          if ( ptable > in ){
            ptable = in;
          }
          //          printf("%d %08x %08x\n",__LINE__,ptable,in);
          CreatInvTable(table,ptable);

          ptr = ptable;

          for(i = 0; i < 0x8000U; ++i) {
#ifdef WORD_ALIGN_NEEDED
            offset1 = (*ptr<<8)| *--ptr; // fetch word , make it offset
#else
            --ptr; offset1 = *(ushort *)ptr; // [1]
#endif
            addrtab[table[offset1]] = i;
            ++table[offset1];
          }

          CreatInvTable(table,ptable);
        }
        if ( in_ptr + 0xfff > in ){          // minimal look-back address
          offset = ptable - in;
        }else {
          offset = ptable  - in_ptr - 0xfff;
        }

        //        printf("off %d\n",offset);
#ifdef WORD_ALIGN_NEEDED
        offset1 = (*in_ptr<<8)|*(in_ptr-1); // fetch current word
#else
        offset1 = *(ushort *)(in_ptr-1); // [1]
#endif
        //        printf("off1 %d\n",offset1);

        offset2 = table[offset1];

        //        printf("off2 %d\n",offset2);

        for( offset2 = table[offset1];
             addrtab[offset2] < offset;
             ++offset2)
        {
          /* empty */
        }

        table[offset1]= offset2;  // next time start searching here !

        ptable += 2;

        in_ptr -= 2;

        uchar d3 = *--in_ptr;
        int bytecount;
        int max_count = 0;
        uchar * sqz_ptr;

        do{
          uchar * ptr;
//
// we want at least a 3 byte sequence
//
          do{
            ptr = ptable - addrtab[offset2++];
          }while ( *--ptr != d3 && ptr > end_notpacked);

          if ( ptr == in_ptr ) break; // same byte ?

          bytecount = 3;
          for( int max = 0; max<15 && (*ptr == in_ptr[max]) ; ++max)
          {
            ++bytecount;
            --ptr;
          }

          if ( bytecount > max_count ) // find best sequence
          {
            max_count = bytecount;
            sqz_ptr = ptr;
          }

        }while ( bytecount != 18 );   // 18 , then no better possible

        ptable -= 2;

        in_ptr = save_in;

        if ( max_count ) {
          printf("%d %d %d\n",__LINE__,max_count,offset);
          *ppackbyte |= 1;            // set flag

          sqz_ptr -= max_count;
//
// pack only till end of data
//
          if ( max_count > in_ptr - end_notpacked )
            max_count = in_ptr - end_notpacked;

          offset = sqz_ptr - in_ptr;  // get offset (but positive !)

          in_ptr -= max_count;        // update unpacked pointer

          max_count -= 3;             // count of 3 implied

          if ( jumptable ){

            // If we use a jumptable, we need an offset rather
            // than a counter.
            *out_ptr++ = (15-max_count) | ((offset >> 4) & 0xf0);
          } else {
            *out_ptr++ = max_count | ((offset >> 4) & 0xf0);
          }
          *out_ptr++ = (uchar) offset & 0xff;

          continue;
        }
        printf("%d %d\n",__LINE__,max_count);
      }
      *out_ptr++ = *--in_ptr; // store byte
    }// for (counter = ...
#if 0
//
// overwrite already packed data to save space
//
    uchar * ptable2 = in_ptr + 0x1000;
    long i;

    if ( (long)ptable2 & 1) ++ptable2;

    if ( ptable2 > in && (i = ptable2 - in0) > 0x8000L )
    {
      memcpy((char *)in0,(const char *)ptable2,out_ptr-ptable2);
      in   -= i;
      end_notpacked -= i;
      in_ptr  -= i;
      out_ptr -= i;
      ptable  -= i;
    }
#endif
//
// check if data is un-packable
//
    if ( out_ptr > in0 + length + 0x8000L)
      return -1;
  }

  if ( counter != 8 )
    *ppackbyte <<= counter; // adjust packbyte
  else
    --out_ptr;              // skip last (unused) packbyte

  memcpy((char *)in0,(const char *)end_notpacked,out_ptr-end_notpacked);
//->  PrintPercentage(out_ptr-end_notpacked,length,in_ptr-in);
  printf("\n");
  return (out_ptr-end_notpacked);
}

void PrintPercentage(long packedlen,long length,long done)
{
  static ushort packed = 1000;
  static ushort ratio  = 1000;

  ++done;

  ushort newpacked = (ushort) (done*100.0/length+0.5);
  ushort newratio  = 100-(ushort)(packedlen*100.0/done+0.5);

  if ( newratio > 100 ) newratio = 0;
  if ( newpacked != packed || newratio != ratio)
  {
    packed = newpacked;
    ratio  = newratio;
    printf("Packed : %3d%%  Ratio : -%3d%%\r",packed,ratio);
    fflush(stdout);
  }
}

void error(const char *s,const char *s1)
{
  printf("TurboPacker " VER " " __DATE__ "\n");
  printf(s,s1);
  printf("\n");
  exit(-1);
}

#define COMMON -1
#define LYNX    0

struct HEADER
{
public:

  void Check(uchar *data,long unpackedlen,bool little);
  void Prepare(uchar *data,long packedlen,uchar **header,long *headlen);
  void PrepareL(uchar *data,long packedlen,uchar **header,long *headlen);

private:

  short type,size;
  void * pheader;

}Header;

struct LYNXHEADER
{
  ushort magic;
  ushort unpacked;
};

struct TPHEADER
{
  ulong magic;
  long unpacked;
};

int main(int argc, char *argv[])
{
  uchar * inbuffer;
  long inlen,outlen;
  bool data = false;
  bool little = false;
  bool jumptable = false;
  ushort * table;
  ushort * addrtab;
  char outfile[256];
  int inv = false;

  if (argc == 1)
    error("usage : tp {[(-|+)d] [(+|-)l] [(+|-)j]file [-o out]}","");

  if ( (table   = (ushort *)malloc(0x10001L * sizeof(ushort))) == NULL ||
       (addrtab = (ushort *)malloc(0x10000L * sizeof(ushort))) == NULL)
    error("Could not allocate memory for tables !","");

  --argc; ++argv; // skip command name

  while( argc )
  {

    if ( !strncmp(*argv,"+d",2))
    {
      data = true;
      ++argv;--argc;
      continue;
    }
    if ( !strncmp(*argv,"-d",2))
    {
      data = false;
      ++argv;--argc;
      continue;
    }
    if ( !strncmp(*argv,"+l",2))
    {
      little = true;
      ++argv;--argc;
      continue;
    }
    if ( !strncmp(*argv,"-l",2))
    {
      little = false;
      ++argv;--argc;
      continue;
    }

    if ( !strncmp(*argv,"+j",2))
    {
      jumptable = true;
      ++argv;--argc;
      continue;
    }
    if ( !strncmp(*argv,"-j",2))
    {
      jumptable = false;
      ++argv;--argc;
      continue;
    }

    if ( !strncmp(*argv,"+i",2))
    {
      inv = true;
      ++argv;--argc;
      continue;
    }


    if ( (inlen = LoadFile(*argv,&inbuffer) ) < 0)
      error("Could not load :%s",*argv);

    printf("Loaded : %s(%ld bytes)\n",*argv,inlen);
    strncpy(outfile,*argv,251);
    ++argv;--argc;

    if ( argc && !strncmp(*argv,"-o",2) )
    {
      ++argv; --argc;
      if ( ! argc )
        error("No output-file after -o !","");
      strncpy(outfile,*argv,255);
      ++argv; --argc;
    }
    else
    {
      char * dotptr;
      if ( ( dotptr = strchr(outfile,'.')) != NULL)
        *dotptr = 0;
      strcat(outfile,".pck");
    }

    Header.Check(inbuffer,inlen,little);

    if ( inlen < 128 ){
      printf("File too short . Skipped !\n");
    } else {
      if (inv == true ){
        outlen = InvPacker(inbuffer,inlen,table,addrtab,jumptable);
      } else {
        outlen = Packer(inbuffer,inlen,table,addrtab,jumptable);
      }
      printf("\n");
      if ( (outlen) && outlen < inlen )  {
        uchar * head;
        long headlen;

        if ( data ) {
          headlen = 0;
        } else {
          Header.Prepare(inbuffer,outlen,&head,&headlen);
        }

        if ( outlen != SaveFile(outfile,inbuffer,outlen,head,headlen))
          error("Could not write %s!",outfile);

        printf("Saved  : %s(%ld bytes)        \n",outfile,outlen+headlen);
      } else {
        printf("in: %ld, out %ld\n",inlen, outlen);
        printf("File skipped ! %20s\n"," ");
      }
    }

    free(inbuffer);
  }
  return 0;
}

/*****************************************************************************/
ulong ReverseEndian(register ulong d)
{
  return  ((d << 24) |
           ((d & 0x0000ff00) << 8) |
           ((d & 0x00ff0000) >> 8) |
           (d >> 24));
}

//
// check file-type and select header
//
void HEADER::Check(uchar *data,long unpackedlen, bool little)
{
  if ( *(ushort *)data == BLL_MAGIC )
  {
    pheader = malloc(size = sizeof(LYNXHEADER));
    ((LYNXHEADER *)pheader)->magic    = BLL_PACKED;
    ((LYNXHEADER *)pheader)->unpacked = ((unpackedlen<<8)&0xff00)|((unpackedlen>>8) &0xff);
    type = LYNX;
    return;
  }

  pheader = malloc(size = sizeof(TPHEADER));
  if ( !little ){
    ((TPHEADER *)pheader)->magic    = TPWM;
    ((TPHEADER *)pheader)->unpacked = LE2BE(unpackedlen);
  } else {
    ((TPHEADER *)pheader)->magic    = LE2BE(TPWM);
    ((TPHEADER *)pheader)->unpacked = unpackedlen;
  }
  type = COMMON;
}
//
// prepare the header for writing
// not much now...
//
void HEADER::Prepare(uchar *data,long packed,uchar **header,long *headlen)
{
  *header = (uchar *)pheader;
  *headlen = size;
}

void HEADER::PrepareL(uchar *data,long packed,uchar **header,long *headlen)
{
  *header = (uchar *)pheader;
  *headlen = LE2BE(size);
}
