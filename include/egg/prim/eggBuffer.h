#ifndef EGG_PRIM_BUFFER_H
#define EGG_PRIM_BUFFER_H

#include <egg/core/eggHeap.h>
#include <egg/prim/eggAssert.h>

namespace EGG {

template <typename T>
class TBuffer {
public:
    class iterator {
    public:
        iterator(TBuffer *pBuffer, int index) : mBuffer(pBuffer), mIndex(index) {}

        int get_index() const {
            return mIndex;
        }

        iterator &operator++() {
            mIndex++;
            return *this;
        }

        T &operator*() {
            return (*mBuffer)(mIndex);
        }

        friend bool operator==(const iterator &rLhs, const iterator &rRhs) {
            return rLhs.mBuffer == rRhs.mBuffer && rLhs.mIndex == rRhs.mIndex;
        }
        friend bool operator!=(const iterator &rLhs, const iterator &rRhs) {
            return !(rLhs == rRhs);
        }

    private:
        TBuffer *mBuffer; // _00
        int mIndex;       // _04
    };

public:
    TBuffer() : mSize(0), mBuffer(nullptr) {}

    virtual ~TBuffer() {
        if (mBuffer == nullptr) {
            return;
        }

        delete[] mBuffer;
        mBuffer = nullptr;
    } 																// _08
    virtual void allocate(int size, int align = 0); 				// _0C
    virtual void allocate(int size, Heap *pHeap, int align = 0); 	// _10
    virtual void onAllocate(Heap*) {} 								// _14
    virtual void errRangeOver() const {}           					// _18

    T &operator()(int i) {
        checkRange(i);
        return mBuffer[i];
    }
    const T &operator()(int i) const {
        checkRange(i);
        return mBuffer[i];
    }

    T &get(int i) {
        checkRange(i);
        return mBuffer[i];
    }
    const T &get(int i) const {
        checkRange(i);
        return mBuffer[i];
    }

    int getSize() const {
        return mSize;
    }
    int size() const {
        return mSize;
    }

    iterator begin() {
        return iterator(this, 0);
    }
    iterator end() {
        return iterator(this, getSize());
    }

private:
    void flush() {}

    void checkRange(int i) const {
        if (!isRangeValid(i)) {
            errRangeOver();
#line 174
           EGG_ASSERT_MSG(false, "TBuffer::checkRange %d (0<=x<%d)\n", i, mSize);
        }
    }

    bool isRangeValid(int i) const {
        return 0 <= i && i < mSize;
    }

private:
    int mSize;  // _04
    T *mBuffer; // _08
};

template <typename T>
void TBuffer<T>::allocate(int size, int) {
    mSize = size;
    mBuffer = new T[size];
    onAllocate(nullptr);
}
template <typename T>
void TBuffer<T>::allocate(int size, Heap *pHeap, int) {
    mSize = size;

    if (pHeap == nullptr) {
        pHeap = Heap::getCurrentHeap();
    }

    mBuffer = new (pHeap) T[mSize];

    flush();
    onAllocate(pHeap);
}

} // namespace EGG


#endif