#ifndef __MATRIXMODE_H__
#define __MATRIXMODE_H__

namespace a3d
{
	namespace MatrixModes
	{
		enum MatrixMode
		{
			VIEW,
			WORLD,
			PROJECTION
		};
	}
	
	typedef MatrixModes::MatrixMode MatrixMode;
}

#endif