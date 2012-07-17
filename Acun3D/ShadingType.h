#ifndef __SHADINGTYPE_H__
#define __SHADINGTYPE_H__

namespace a3d
{
	namespace ShadingTypes
	{
		enum ShadingType
		{
			NONE,
			FLAT,
			SMOOTH,
			PHONG
		};
	}

	typedef ShadingTypes::ShadingType ShadingType;
}

#endif
