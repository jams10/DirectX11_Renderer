#pragma once

#define NAMESPACE renderer

namespace NAMESPACE
{
	#define __ERRORLINE__ cout << "Line : " << __LINE__ << " / File : " << __FILE__ << '\n';
	#define SAFE_RELEASE(p) if(p) { delete p; p = nullptr; }
}
