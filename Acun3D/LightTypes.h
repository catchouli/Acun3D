#ifndef __LIGHTTYPES_H__
#define __LIGHTTYPES_H__

namespace a3d
{
	namespace LightTypes
	{
		enum LightType
		{
			POINT,
			DIRECTIONAL,
			AMBIENT,
			SPOT
		};
	}

	typedef LightTypes::LightType LightType;
}
#endif