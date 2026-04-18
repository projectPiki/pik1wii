#ifndef EGG_CORE_STREAM_H
#define EGG_CORE_STREAM_H

#include <egg/egg_types.h>

#include <egg/core/eggBitFlag.h>

namespace EGG {

/******************************************************************************
 *
 * Stream
 *
 ******************************************************************************/

class Stream {
public:
    Stream();
    
	virtual ~Stream(); 								// _08
    virtual void read(u8* pDst, u32 size) = 0;  	// _0C
    virtual void write(u8* pSrc, u32 size) = 0; 	// _10
    virtual bool eof() = 0;                     	// _14
    virtual void beginMemoryMap(char*);         	// _18
    virtual void endMemoryMap(char*);           	// _1C
    virtual u32 peek_u32() = 0;                 	// _20

/**
 * @brief Helper for declaring stream functions for primitive types
 */
#define TYPE_FUNC_DECL(T)                                                      \
    T read_##T();                                                              \
    void write_##T(T value);                                                   \
    void read_##T(T& rValue) {                                                 \
        rValue = read_##T();                                                   \
    }

    TYPE_FUNC_DECL(u8);
    TYPE_FUNC_DECL(s8);
    TYPE_FUNC_DECL(u16);
    TYPE_FUNC_DECL(s16);
	TYPE_FUNC_DECL(u32);
    TYPE_FUNC_DECL(s32);
	TYPE_FUNC_DECL(u64);
    TYPE_FUNC_DECL(float);

#undef TYPE_FUNC_DECL

    f32 read_f32() {
        return read_float();
    }
    void write_f32(f32 value) {
        write_float(value);
    }
    void read_f32(f32& rValue) {
        rValue = read_f32();
    }
	
	u32 read_u32_hex();
    void write_u32_hex(u32 value);
    void read_u32_hex(u32& rValue);

    const char* readString(char* pDst, int maxlen);
    void writeString(char* pStr);

    bool isSpace(char ch);
    bool isCRLF(char ch);
    bool isUpperSJIS(char ch);
	
	bool isBeginGroup();
    bool isEndGroup();

    void setTextMode() {
        mFlags.setBit(BIT_TEXT_MODE);
    }
    bool isTextMode() {
        return mFlags.onBit(BIT_TEXT_MODE);
    }
    void setBinMode() {
        mFlags.resetBit(BIT_TEXT_MODE);
    }
    bool isBinMode() {
        return !isTextMode();
    }

    s32 getPosition() const {
        return mPosition;
    }

protected:
    bool mIsTokenReady;    // _04
    s32 mPosition;         // _08
    TBitFlag<u16> mFlags;  // _0C
    char* mTextBuffer;     // _10
    s32 mTextBufferSize;   // _14
    u32 mGroupIndentLevel; // _18
    bool mAllowComments;   // _1C

    static const int TEXT_BUFFER_SIZE = 1024;
    static char sTextBuffer[TEXT_BUFFER_SIZE];

private:
    enum {
        BIT_TEXT_MODE,
    };

private:
    void skipUntilCRLF();
    void copyToTextBuffer();
    const char* getNextToken();

    char skipSpace();
    void printf(char* pFmt, ...);

    void _read(void* pDst, u32 size);
    void _write(void* pSrc, u32 size);

    u8 _readByte();
    void _writeByte(u8 value);
};

/******************************************************************************
 *
 * RamStream
 *
 ******************************************************************************/

class RamStream : public Stream {
public:
    RamStream(u8* pBuffer, u32 size);

    virtual void read(u8* pDst, u32 size);  	// _0C
    virtual void write(u8* pSrc, u32 size); 	// _10
    virtual bool eof();                     	// _14
    virtual u32 peek_u32();                 	// _20

private:
    u8* mBuffer;     // _20
    u32 mBufferSize; // _24
};

} // namespace EGG

#endif
