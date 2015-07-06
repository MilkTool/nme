#ifndef AUDIO_H
#define AUDIO_H

#include <nme/QuickVec.h>
#include <Sound.h>
#include <Utils.h>

#ifdef ANDROID
#include <android/log.h>
#endif


#if 0
   #ifdef ANDROID
      #define LOG_SOUND(args,...) ELOG(args, ##__VA_ARGS__);
   #elif defined(IPHONE)
      #define LOG_SOUND(args,...) NmeLog(args, ##__VA_ARGS__);
   #elif defined(TIZEN)
      #include <FBase.h>
      #define LOG_SOUND(args,...) AppLog(args, ##__VA_ARGS__);
   #else
      #define LOG_SOUND(args,...) printf(args, ##__VA_ARGS__);
   #endif
#else
   #define LOG_SOUND(args,...)  { }
#endif


namespace nme
{

class INmeSoundData;
class INmeSoundStream;

enum
{
   SoundForceDecode = 0x0001,
   SoundJustInfo    = 0x0002,
};


class INmeSoundData
{
public:
   static INmeSoundData *create(const std::string &inId, unsigned int inFlags=0x0000);
   static INmeSoundData *create(const unsigned char *inData, int inDataLength, unsigned int inFlags=0x0000);
   static INmeSoundData *create(const short *inData, int inChannelSamples, bool inIsStereo, int inRate);

   virtual INmeSoundData  *addRef() = 0;
   virtual void   release() = 0;
   virtual double getDuration() const = 0;
   virtual int    getChannelSampleCount() const = 0;
   virtual bool   getIsStereo() const = 0;
   virtual int    getRate() const = 0;
   virtual bool   getIsDecoded() const = 0;
   virtual short  *decodeAll() = 0;
   virtual INmeSoundStream *createStream()=0;

   virtual bool isValid() const { return getChannelSampleCount(); }
protected:
   // Call "release"
   ~INmeSoundData() { }
};

enum NmeStreamState
{
   nssOk,
   nssNeedsSyncData,
   nssEof,
};


class INmeSoundStream 
{
public:
   virtual ~INmeSoundStream() { }

   virtual double getPosition() = 0;
   virtual void   setPosition(double inSeconds) = 0;
   virtual void   rewind() = 0;
   virtual double getDuration() const = 0;
   virtual int    getRate() const = 0;
   virtual int    getChannelSampleCount() const = 0;
   virtual bool   getIsStereo() const = 0;
   virtual bool   isValid() const { return getChannelSampleCount(); }

   virtual int    fillBuffer(char *outBuffer, int inRequestBytes) = 0;
   virtual NmeStreamState getDataState() const { return nssEof; }
};




enum AudioFormat
{
   eAF_unknown,
   eAF_ogg,
   eAF_wav,
   eAF_mid,
   eAF_count
};

AudioFormat determineFormatFromBytes(const unsigned char *inData, int len);
AudioFormat determineFormatFromFile(const std::string &filename);

bool loadOggSampleFromBytes(const unsigned char *inData, int len, QuickVec<unsigned char> &outBuffer, int *channels, int *bitsPerSample, int* outSampleRate);
bool loadOggSampleFromFile(const char *inFileURL, QuickVec<unsigned char> &outBuffer, int *channels, int *bitsPerSample, int* outSampleRate);

bool loadWavSampleFromBytes(const unsigned char *inData, int len, QuickVec<unsigned char> &outBuffer, int *channels, int *bitsPerSample, int* outSampleRate);
bool loadWavSampleFromFile(const char *inFileURL, QuickVec<unsigned char> &outBuffer, int *channels, int *bitsPerSample, int* outSampleRate);




   class AudioStream
   {
   public:
      static AudioStream *createOgg();
      virtual ~AudioStream() {}
      
      virtual bool open(const std::string &path, int startTime)=0;
      virtual double getLength(const std::string &path) = 0;
      virtual double getPosition() = 0;
      virtual double setPosition(const float &inFloat) = 0;
      virtual int fillBuffer(char *outBuffer, int inRequestBytes) = 0;
      virtual void rewind() = 0;
      virtual int getRate() = 0;
      virtual bool isValid() = 0;

   };
   
}

#endif