#include "System12/TagParms.h"

namespace System12 {

template <>
void PrimTagParm<int>::doRead(EGG::Stream& stream)
{
	mData = stream.read_s32();
}

template <>
void PrimTagParm<int>::doWrite(EGG::Stream& stream)
{
	stream.write_s32(mData);
}

template <>
void PrimTagParm<int>::doDump()
{
}

template <>
void PrimTagParm<u32>::doRead(EGG::Stream& stream)
{
	mData = stream.read_u32_hex();
}

template <>
void PrimTagParm<u32>::doWrite(EGG::Stream& stream)
{
	stream.write_u32_hex(mData);
}

template <>
void PrimTagParm<u32>::doDump()
{
}

template <>
void PrimTagParm<f32>::doRead(EGG::Stream& stream)
{
	mData = stream.read_float();
}

template <>
void PrimTagParm<f32>::doWrite(EGG::Stream& stream)
{
	stream.write_float(mData);
}

template <>
void PrimTagParm<f32>::doDump()
{
}

StringTagParm::StringTagParm(TagParameters* params, char* name)
    : TagParm(params, name)
{
	mData = nullptr;
}

void StringTagParm::flushValue() { }


void StringTagParm::doRead(EGG::Stream& stream) { mData = const_cast<char*>(stream.readString(nullptr, 0)); }

void StringTagParm::doWrite(EGG::Stream& stream) { stream.writeString(const_cast<char*>(mData)); }


void StringTagParm::doDump() { }

TagParameters::TagParameters(char* name)
{
	mHead = nullptr;
	mName = name;
}

TagParm::TagParm(TagParameters* container, char* name)
{
	mName = name;
	mNext = nullptr;
	if (container->mHead) {
		mNext            = container->mHead;
		container->mHead = this;
		return;
	}

	container->mHead = this;
}

void TagParameters::read(EGG::Stream& stream)
{
	stream.isBeginGroup();
	while (true) {
		if (stream.isEndGroup()) {
			break;
		}
		char* str  = const_cast<char*>(stream.readString(nullptr, 0));
		int strLen;

		for (TagParm* node = mHead; node; node = node->mNext) {
			strLen          = strlen(str);
			int nodeNameLen = strlen(node->mName);
			if (strLen != nodeNameLen) {
				continue;
			}

			strLen = strlen(str);
			if (strncmp(node->mName, str, strLen) == 0) {
				node->doRead(stream);
			}
		}
	}
}

}