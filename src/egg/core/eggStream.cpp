#include <egg/core/eggStream.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace EGG {

/******************************************************************************
 *
 * Stream
 *
 ******************************************************************************/

char Stream::sTextBuffer[TEXT_BUFFER_SIZE];

Stream::Stream() : mIsTokenReady(false) {
    mPosition = 0;
    mGroupIndentLevel = 0;

    mTextBufferSize = sizeof(sTextBuffer);
    mTextBuffer = sTextBuffer;

    mAllowComments = false;
}

Stream::~Stream() {}

DECOMP_FORCEACTIVE(eggStream_cpp,
                  "skippadding : %x (%d bytes)\n");
u8 Stream::read_u8() {
    u8 value;
    int number;

    if (isTextMode()) {
        const char* pToken = getNextToken();

        sscanf(pToken, "%d", &number);
        return static_cast<u8>(number);
    }

    _read(&value, sizeof(u8));
    return value;
}

DECOMP_FORCEACTIVE(eggStream_cpp,
                  "skipped : %d -> %d (%d)\n");
				  
void Stream::write_u8(u8 value) {
    if (isTextMode()) {
        printf("%d ", value);
    } else {
        _write(&value, sizeof(u8));
    }
}

s8 Stream::read_s8() {
    s8 value;
    int number;

    if (isTextMode()) {
        const char* pToken = getNextToken();

        sscanf(pToken, "%d", &number);
        return static_cast<s8>(number);
    }

    _read(&value, sizeof(s8));
    return static_cast<s8>(value);
}

void Stream::write_s8(s8 value) {
    if (isTextMode()) {
        printf("%d ", value);
    } else {
        _write(&value, sizeof(s8));
    }
}

u16 Stream::read_u16() {
    u16 value;
    int number;

    if (isTextMode()) {
        const char* pToken = getNextToken();
		
        sscanf(pToken, "%d", &number);
        return static_cast<u16>(number);
    }

    _read(&value, sizeof(u16));
    return value;
}

void Stream::write_u16(u16 value) {
    if (isTextMode()) {
        printf("%d ", value);
    } else {
        _write(&value, sizeof(u16));
    }
}

s16 Stream::read_s16() {
    s16 value;
    int number;

    if (isTextMode()) {
        const char* pToken = getNextToken();
		
        sscanf(pToken, "%d", &number);
        return static_cast<s16>(number);
    }

    _read(&value, sizeof(s16));
    return value;
}

void Stream::write_s16(s16 value) {
    if (isTextMode()) {
        printf("%d ", value);
    } else {
        _write(&value, sizeof(s16));
    }
}
				  
s32 Stream::read_s32() {
    s32 value;
    int number;

    if (isTextMode()) {
        const char* pToken = getNextToken();

        sscanf(pToken, "%d", &number);
        return static_cast<s32>(number);
    }

    _read(&value, sizeof(s32));
    return value;
}

void Stream::write_s32(s32 value) {
    if (isTextMode()) {
        printf("%d ", value);
    } else {
        _write(&value, sizeof(s32));
    }
}

u32 Stream::read_u32_hex() {
    u32 value;
    int number;

    if (isTextMode()) {
        const char* pToken = getNextToken();

        sscanf(pToken, "%x", &number);
        return static_cast<u32>(number);
    }

    _read(&value, sizeof(u32));
    return value;
}

void Stream::write_u32_hex(u32 value) {
    if (isTextMode()) {
        printf("%x ", value);
    } else {
        _write(&value, sizeof(u32));
    }
}

u64 Stream::read_u64() {
    u64 value;
    int number;

    if (isTextMode()) {
        const char* pToken = getNextToken();

        sscanf(pToken, "%llu", &number);
        return static_cast<u64>(number);
    }

    _read(&value, sizeof(u64));
    return value;
}

void Stream::write_u64(u64 value) {
    if (isTextMode()) {
        printf("%llu ", value);
    } else {
        _write(&value, sizeof(u64));
    }
}

DECOMP_FORCEACTIVE(eggStream_cpp,
                  "%04x ",
                  "%08x ");

f32 Stream::read_float() {
    f32 value;
    f32 number;

    if (isTextMode()) {
        const char* pToken = getNextToken();

        sscanf(pToken, "%f ", &number);
        return number;
    }

    _read(&value, sizeof(f32));
    return value;
}

void Stream::write_float(f32 value) {
    if (isTextMode()) {
        printf("%f ", value);
    } else {
        _write(&value, sizeof(f32));
    }
}

DECOMP_FORCEACTIVE(eggStream_cpp_1,
                  "%s");

void Stream::writeString(char* pStr) {
    int len = strlen(pStr);

    if (isTextMode()) {
        bool escape = false;

        for (int i = 0; i < len; i++) {
            if (isUpperSJIS(pStr[i])) {
                i++;
                continue;
            }

            if (isSpace(pStr[i])) {
                escape = true;
            }
        }

        if (escape) {
            _writeByte('\"');

            for (int i = 0; i < len; i++) {
                if (isUpperSJIS(pStr[i])) {
                    _writeByte(pStr[i]);
                    _writeByte(pStr[i + 1]);
                    i++;
                } else if (pStr[i] == '\"') {
                    _writeByte('\\');
                    _writeByte('\"');
                } else {
                    _writeByte(pStr[i]);
                }
            }

            _writeByte('\"');
        } else {
            for (int i = 0; i < len; i++) {
                if (isUpperSJIS(pStr[i])) {
                    _writeByte(pStr[i]);
                    _writeByte(pStr[i + 1]);
                    i++;
                } else {
                    _writeByte(pStr[i]);
                }
            }
        }
    } else {
        for (int i = 0; i < len; i++) {
            _writeByte(pStr[i]);
        }

        _writeByte('\0');
    }
}

const char* Stream::readString(char* pDst, int maxLen) {
    char buffer[TEXT_BUFFER_SIZE];

    // Token cached
    if (mIsTokenReady) {
        u32 textLen = strlen(mTextBuffer);

        if (pDst == nullptr) {
            pDst = new char[textLen + 1];
        }

        for (int i = 0; i < textLen + 1; i++) {
            pDst[i] = mTextBuffer[i];
        }

        mIsTokenReady = false;
        return pDst;
    }

    // Text mode (no token cached)
    if (isTextMode()) {
        char delim = skipSpace();
        if (delim == '\"') {
            int size, pos;
            for (size = 0, pos = 0; pos < TEXT_BUFFER_SIZE; pos++) {
                u8 byte = _readByte();

                if (byte == '\"') {
                    break;
                }

                if (byte == '\\') {
					_readByte();
                    buffer[size++] = '\"';
                } else if (isUpperSJIS(byte)) {
                    buffer[size++] = byte;
                    buffer[size++] = _readByte();
                } else {
                    buffer[size++] = byte;
                }
            }

            buffer[size] = '\0';

            int textLen = strlen(buffer);
            if (pDst != nullptr) {
            } else {
                pDst = new char[textLen + 1];
            }

            for (int i = 0; i < textLen + 1; i++) {
                pDst[i] = buffer[i];
            }

            return pDst;
        }

        int size, pos;
        for (size = 0, pos = 0; pos < TEXT_BUFFER_SIZE; pos++) {
            if (eof()) {
                break;
            }

            if (isSpace(delim)) {
                if (mAllowComments || delim != '#') {
                    break;
                }

                skipUntilCRLF();
                break;
            } else if (isUpperSJIS(delim)) {
                buffer[size++] = delim;
                buffer[size++] = _readByte();
            } else {
                buffer[size++] = delim;
            }

            delim = _readByte();
        }

        buffer[size] = '\0';

        int textLen = strlen(buffer);
		if (pDst != nullptr) {
        } else {
            pDst = new char[textLen + 1];
        }


        for (int i = 0; i < textLen + 1; i++) {
            pDst[i] = buffer[i];
        }

        return pDst;
    }

    // Binary mode
    for (int i = 0; i < TEXT_BUFFER_SIZE; i++) {
        u8 byte = _readByte();
        u32 idx = static_cast<u32>(i);

        buffer[idx] = byte;

        if (byte == '\0') {
            break;
        }
    }

    int textLen = strlen(buffer);
    if (pDst != nullptr) {
    } else {
        pDst = new char[textLen + 1];
    }

    for (int i = 0; i < textLen + 1; i++) {
        pDst[i] = buffer[i];
    }

    return pDst;
}

void Stream::_read(void* pDst, u32 size) {
	eof();
    read(static_cast<u8*>(pDst), size);
    mPosition += size;
}

void Stream::_write(void* pSrc, u32 size) {
    write(static_cast<u8*>(pSrc), size);
    mPosition += size;
	eof();
}

u8 Stream::_readByte() {
    u8 value;
    _read(&value, sizeof(u8));
    return value;
}

void Stream::_writeByte(u8 value) {
    _write(&value, sizeof(u8));
}

void Stream::skipUntilCRLF() {
    while (!eof()) {
        char ch = _readByte();

        if (isUpperSJIS(ch)) {
            ch = _readByte();
        } else if (ch == '\r' || ch == '\n') {
            return;
        }
    }
}

void Stream::copyToTextBuffer() {
    *mTextBuffer = skipSpace();

    int i = 1;
    while (!eof() && i < mTextBufferSize) {
        char ch = _readByte();

        if (isSpace(ch)) {
            mTextBuffer[i] = '\0';

            if (!mAllowComments && ch == '#') {
                skipUntilCRLF();
            }

            return;
        } else {
            mTextBuffer[i++] = ch;

            if (ch == '\0') {
                return;
            }
        }
    }
}

const char* Stream::getNextToken() {
    if (!isTextMode()) {
        return nullptr;
    }

    if (mIsTokenReady) {
        mIsTokenReady = false;
        return mTextBuffer;
    }

    copyToTextBuffer();
    return mTextBuffer;
}

bool Stream::isSpace(char ch) {
    return ch == '\t' || ch == '\n' || ch == '\r' || ch == ' ' ||
           !mAllowComments && ch == '#' || ch == '!';
}

bool Stream::isCRLF(char ch) {
    return ch == '\r' || ch == '\n';
}

bool Stream::isUpperSJIS(char ch) {
    u8 byte = static_cast<u8>(ch);
    return byte >= 0x81 && byte <= 0x9F || byte >= 0xE0 && byte <= 0xFC;
}

char Stream::skipSpace() {
    if (isTextMode()) {
        bool skip = false;

        while (!eof()) {
            char ch = _readByte();

            if (skip) {
                if (isUpperSJIS(ch)) {
                    ch = _readByte();
                } else if (isCRLF(ch)) {
                    skip = false;
                }
            } else if (!mAllowComments && ch == '#' || ch == '!') {
                skip = true;
            } else if (!isSpace(ch)) {
                return ch;
            }
        }
    }

    return '\0';
}

void Stream::printf(char* pFmt, ...) {
    char buffer[TEXT_BUFFER_SIZE];

    va_list list;
    va_start(list, pFmt);
    vsprintf(buffer, pFmt, list);
    va_end(list);

    int len = strlen(buffer);
    if (len > 0) {
        for (int i = 0; i < len; i++) {
            _writeByte(buffer[i]);
        }
    }
}

/******************************************************************************
 *
 * RamStream
 *
 ******************************************************************************/

RamStream::RamStream(u8* pBuffer, u32 size)
    : mBuffer(pBuffer), mBufferSize(size) {}

void RamStream::read(u8* pDst, u32 size) {
    for (int i = 0; i < size; i++) {
        pDst[i] = mBuffer[mPosition + i];
    }
}

u32 RamStream::peek_u32() {
	u8 data[4] = {mBuffer[getPosition()], mBuffer[getPosition() + 1],
                mBuffer[getPosition() + 2], mBuffer[getPosition() + 3]};

	return *reinterpret_cast<u32*>(data);
}

void RamStream::write(u8* pSrc, u32 size) {
    for (int i = 0; i < size; i++) {
        mBuffer[mPosition + i] = pSrc[i];
    }
}

bool RamStream::eof() {
    return mPosition > mBufferSize;
}


} // namespace EGG