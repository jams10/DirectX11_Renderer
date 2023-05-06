
#include "AppBase.h"
#include <iostream>

namespace NAMESPACE
{
	using std::cout;

	AppBase::AppBase(int screenWidth, int screenHeight)
		:
		m_screenWidth(screenWidth),
		m_screenHeight(screenHeight),
		window()
	{
	}

	AppBase::~AppBase()
	{
	}

	bool AppBase::Initialize()
	{
		if (window.Initialize(L"Renderer", m_screenWidth, m_screenHeight) == false)
		{
			cout << "Failed to Initialize Window\n";
			__ERRORLINE__
			return false;
		}
		return true;
	}

	int AppBase::Run()
	{
		while (true)
		{
			if (window.ProcessMessages() == 0)
				return 0;

			Render();
		}
	}

}


