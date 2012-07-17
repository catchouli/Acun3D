#ifndef __MATERIALTYPE_H__
#define __MATERIALTYPE_H__

namespace a3d
{
	namespace MaterialTypes
	{
		enum MaterialType
		{
			WIREFRAME,
			SOLID,
			TEXTURED
		};
	}

	typedef MaterialTypes::MaterialType MaterialType;
}

#endif
