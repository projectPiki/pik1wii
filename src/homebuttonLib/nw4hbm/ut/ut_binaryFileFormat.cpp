#include "homebuttonLib/nw4hbm/ut/binaryFileFormat.h"
#include "homebuttonLib/nw4hbm/ut/inlines.h"
#include <types.h>


namespace nw4hbm { 
namespace ut {

bool IsValidBinaryFile(BinaryFileHeader const *header, byte4_t signature,
                       u16 version, u16 minBlocks)
{
	if (header->signature != signature)
		return false;

	// U+FEFF * BYTE ORDER MARK
	if (header->byteOrder != 0xFEFF)
		return false;

	if (header->version != version)
		return false;

	if (header->fileSize
	    < sizeof *header + sizeof(BinaryBlockHeader) * minBlocks)
	{
		return false;
	}

	if (header->dataBlocks < minBlocks)
		return false;

	return true;
}

bool IsReverseEndianBinaryFile(BinaryFileHeader const *fileHeader)
{
	// U+FEFF * BYTE ORDER MARK
	return fileHeader->byteOrder != 0xFEFF;
}

BinaryBlockHeader *GetNextBinaryBlockHeader(BinaryFileHeader *fileHeader,
                                            BinaryBlockHeader *blockHeader)
{
	void *ptr;

	if (!IsReverseEndianBinaryFile(fileHeader))
	{
		if (!blockHeader)
		{
			if (fileHeader->dataBlocks == 0)
				return nullptr;

			ptr = AddOffsetToPtr(fileHeader, fileHeader->headerSize);
		}
		else
		{
			ptr = AddOffsetToPtr(blockHeader, blockHeader->size);
		}

		if (ptr >= AddOffsetToPtr(fileHeader, fileHeader->fileSize))
			return nullptr;
	}
	else
	{
		if (!blockHeader)
		{
			if (fileHeader->dataBlocks == 0)
				return nullptr;

			ptr = AddOffsetToPtr(fileHeader,
			                     ReverseEndian(fileHeader->headerSize));
		}
		else
		{
			ptr = AddOffsetToPtr(blockHeader, ReverseEndian(blockHeader->size));
		}

		if (ptr
		    >= AddOffsetToPtr(fileHeader, ReverseEndian(fileHeader->fileSize)))
		{
			return nullptr;
		}
	}

	return static_cast<BinaryBlockHeader *>(ptr);
}

} // namespace ut
} // namespace nw4hbm
