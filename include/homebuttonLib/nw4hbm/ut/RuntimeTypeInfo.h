#ifndef NW4HBM_UT_RUNTIME_TYPE_INFO_H
#define NW4HBM_UT_RUNTIME_TYPE_INFO_H

#include <types.h>

namespace nw4hbm { 
namespace ut {
namespace detail {

class RuntimeTypeInfo {
public:
	RuntimeTypeInfo(RuntimeTypeInfo const *parent)
		:mParentTypeInfo(parent)
		{
		}

	bool IsDerivedFrom(RuntimeTypeInfo const *typeInfo) const
	{
		RuntimeTypeInfo const *self;

		for (self = this; self; self = self->mParentTypeInfo)
		{
			if (self == typeInfo)
				return true;
			}

			return false;
		}


private:
	RuntimeTypeInfo	const *mParentTypeInfo;	// _00
};

template <class T>
inline RuntimeTypeInfo const *GetTypeInfoFromPtr_(T *)
{
	return &T::typeInfo;
}
} // namespace detail

template <typename U, class T>
inline U DynamicCast(T *obj)
{
	detail::RuntimeTypeInfo const *typeInfoU =
		detail::GetTypeInfoFromPtr_(static_cast<U>(nullptr));

	if (obj->GetRuntimeTypeInfo()->IsDerivedFrom(typeInfoU))
		return reinterpret_cast<U>(obj);

	return nullptr;
}
} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_RUNTIME_TYPE_INFO_H
