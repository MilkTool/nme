#ifndef NME_PIXEL_H
#define NME_PIXEL_H

// The order or RGB or BGR is determined to the primary surface's
//  native order - this allows most transfers to be donw without swapping R & B
// When rendering from a source to a dest, the source is swapped to match in
//  the blending code.

namespace nme
{


// Component order refers to byte-layout in memory
enum PixelFormat
{
   pfNone       = 0,
   // 3 Bytes per pixel
   pfRGB        = 1,
   // 0xAARRGGBB on little-endian = flash native format
   // This can generally be loaded right into the GPU (android simulator - maybe not)
   pfBGRA       = 2,
   // Has the B,G,R components multiplied by A
   pfBGRPremA   = 3,

   // This format is only used to transfer data to the GPU on systems that do
   //  not support the preferred pfBGRPremA format
   pfRGBPremA   = 4,
   pfRGBA       = 5,

   pfAlpha      = 6,

   pfRenderToCount = 7,

   pfARGB4444   = 7,
   pfRGB565,
   pfLuma,
   pfLumaAlpha,
   pfECT,
   pfRGB32f,
   pfRGBA32f,
   pfYUV420sp,
   pfNV12,
   pfOES,
   pfRenderBuffer,
};

enum PixelChannel
{
   CHAN_ALPHA = 0x0008,
   CHAN_BLUE  = 0x0004,
   CHAN_GREEN = 0x0002,
   CHAN_RED   = 0x0001,
};

enum { CHANNEL_OFFSET_VIRTUAL_ALPHA = -1, CHANNEL_OFFSET_NONE = -2 };
int GetPixelChannelOffset(PixelFormat inFormat, PixelChannel inChannel);


typedef unsigned char Uint8;


struct ARGB
{
   inline ARGB() { }
   inline ARGB(int inRGBA) { ival = inRGBA; }
   inline ARGB(int inRGB,int inA) { ival = (inRGB & 0xffffff) | (inA<<24); }
   inline ARGB(int inRGB,float inA)
   {
      ival = (inRGB & 0xffffff);
      int alpha = 255.9 * inA;
      a = alpha<0 ? 0 : alpha >255 ? 255 : alpha;
   }

   inline float getRedFloat() { return r/255.0; }
   inline float getGreenFloat() { return g/255.0; }
   inline float getBlueFloat() { return b/255.0; }
   inline float getAlphaFloat() { return a/255.0; }

   inline int ToInt() const { return ival; }
   inline void Set(int inVal) { ival = inVal; }
   inline void SetRGB(int inVal) { ival = inVal | 0xff000000; }
   inline void SetRGBA(int inVal) { ival = inVal; }
   inline int luma() { return (r + (g<<1) + b + 2) >> 8; }

   template<bool DEST_ALPHA>
   inline void Blend(const ARGB &inVal)
   {
      int A = inVal.a + (inVal.a>>7);
      if (A>5)
      {
         // Replace if input is full, or we are empty
         if (A>250 || (DEST_ALPHA && a<5) )
         {
            ival = inVal.ival;
         }
         // Our alpha is implicitly 256 ...
         else if (!DEST_ALPHA)
         {
            int f = 256-A;
            r = (A*inVal.r + f*r)>>8;
            g = (A*inVal.g + f*g)>>8;
            b = (A*inVal.b + f*b)>>8;
         }
         else
         {
            int alpha16 = ((a + A)<<8) - a*A;
            int f = (256-A) * a;
            A<<=8;
            r = (A*inVal.r + f*r)/alpha16;
            g = (A*inVal.g + f*g)/alpha16;
            b = (A*inVal.b + f*b)/alpha16;
            a = alpha16>>8;
         }
      }
   }


   inline void QBlend(ARGB inVal)
   {
      int A = inVal.a + (inVal.a>>7);
      int f = (256-A);
      b = (A*inVal.b + f*b)>>8;
      g = (A*inVal.g + f*g)>>8;
      r = (A*inVal.r + f*r)>>8;
   }

   inline void QBlendA(ARGB inVal)
   {
      int A = inVal.a + (inVal.a>>7);
      int alpha16 = ((a + A)<<8) - a*A;
      int f = (256-A) * a;
      A<<=8;
      b = (A*inVal.b + f*b)/alpha16;
      g = (A*inVal.g + f*g)/alpha16;
      r = (A*inVal.r + f*r)/alpha16;
      a = alpha16>>8;
   }

   inline void TBlend_0(const ARGB &inVal) { Blend<false>(inVal); }
   inline void TBlend_1(const ARGB &inVal) { Blend<true >(inVal); }

   union
   {
      struct { Uint8 b,g,r,a; };
      unsigned int  ival;
   };
};


inline void BlendAlpha(Uint8 &ioDest, Uint8 inSrc)
{
   if (inSrc)
   {
      if (inSrc==255)
         ioDest = 255;
      else
         ioDest = 255 - ((255 - inSrc) * (255-ioDest) >> 8);
   }
}

inline void BlendAlpha(Uint8 &ioDest, const ARGB &inSrc)
{
   if (inSrc.a)
   {
      if (inSrc.a==255)
         ioDest = 255;
      else
         ioDest = 255 - ((255 - inSrc.a) * (255-ioDest) >> 8);
   }
}


inline void QBlendAlpha(Uint8 &ioDest, Uint8 inSrc)
{
   ioDest = 255 - ((255 - inSrc) * (255-ioDest) >> 8);
}


void PixelConvert(int inWidth, int inHeight,
       PixelFormat srcFormat,  const void *srcPtr, int srcByteStride, int srcPlaneOffset,
       PixelFormat destFormat, void *destPtr, int destByteStride, int destPlaneOffset );

int BytesPerPixel(PixelFormat inFormat);


} // end namespace nme

#endif
