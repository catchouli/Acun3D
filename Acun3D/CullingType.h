#ifndef __CULLINGTYPE_H__
#define __CULLINGTYPE_H__

namespace a3d
{
	namespace CullingTypes
	{
		enum CullingType
		{
			NONE,
			FRONT,
			BACK
		};
	}

	typedef CullingTypes::CullingType CullingType;
}

#endif
