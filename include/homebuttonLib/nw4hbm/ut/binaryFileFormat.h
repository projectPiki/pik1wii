#ifndef NW4HBM_UT_BINARY_FILE_HEADER_H
#define NW4HBM_UT_BINARY_FILE_HEADER_H

#include <types.h>

namespace nw4hbm { 
namespace ut {

struct BinaryFileHeader {
	u32	signature;	// _00
	u16	byteOrder;	// _04
	u16	version;		// _06
	u32	fileSize;		// _08
	u16	headerSize;		// _0C
	u16	dataBlocks;		// _0E
};

struct BinaryBlockHeader {
	u32	kind;	// _00
	u32	size;		// _04
};

	bool IsValidBinaryFile(BinaryFileHeader const *header, u32 signature,
	                       u16 version, u16 minBlocks);
	bool IsReverseEndianBinaryFile(BinaryFileHeader const *fileHeader);
	BinaryBlockHeader *GetNextBinaryBlockHeader(BinaryFileHeader *fileHeader,
	                                            BinaryBlockHeader *blockHeader);

} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_BINARY_FILE_HEADER_H
