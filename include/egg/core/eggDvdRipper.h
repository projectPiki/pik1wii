#ifndef EGG_CORE_DVDRIPPER_H
#define EGG_CORE_DVDRIPPER_H

#include <egg/egg_types.h>

namespace EGG {

////////////////////// DVD RIPPER //////////////////////

/**
 * @brief Namespace-like class for loading data from the DVD.
 *
 * @note Size: 0x0.
 */
class DvdRipper {
public:
	class Arg {
	public:
		Arg()
		{
			_04 = 0;
			_08 = 0;
			_0C = 1;
			_10 = 0;
			_14 = _18;
		}

	private:
		u32 _00;
		u32 _04;
		u32 _08;
		u32 _0C;
		u32 _10;
		u8* _14;
		u8 _18[8];
	};

	typedef void (*DvdRipperCallback)(const Arg& arg);

	/**
	 * @brief Heap allocation end.
	 */
	enum EAllocDirection {
		ALLOC_DIR_PAD,  // 0
		ALLOC_DIR_HEAD, // 1
		ALLOC_DIR_TAIL, // 2
	};

public:
	static u8* loadToMainRAM(const char* pPath, u8* pBuffer, Heap* pHeap, EAllocDirection allocDir, u32 offset, u32* pRead, u32* pSize);

	static u8* loadToMainRAM(DvdFile* pFile, u8* pBuffer, Heap* pHeap, EAllocDirection allocDir, u32 offset, u32* pRead, u32* pSize);

private:
	static DvdRipperCallback sCallback;
	static bool sErrorRetry;
};

////////////////////////////////////////////////////////

} // namespace EGG

#endif
