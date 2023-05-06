#include "ExampleApp.h"
#include <Graphics/Graphics.h>
#include <iostream>

namespace NAMESPACE
{
	using std::cout;

	ExampleApp::ExampleApp(int screenWidth, int screenHeight) : AppBase(screenWidth, screenHeight) 
	{
		graphics = nullptr;
	}

	bool ExampleApp::Initialize()
	{
		if (AppBase::Initialize() == false)
			return false;

		graphics = new Graphics();
		if (graphics->Initialize(m_screenWidth, m_screenHeight, window.GetWindowHandle()) == false)
		{
			cout << "Failed : Graphics Initialize.\n";
			__ERRORLINE__
			return false;
		}

		return true;
	}

	void ExampleApp::UpdateGUI()
	{
	}

	void ExampleApp::Update(float dt)
	{
	}

	void ExampleApp::Render()
	{
		if(graphics != nullptr)
			graphics->Render(0);
		
	}
}

