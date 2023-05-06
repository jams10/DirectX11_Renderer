#include "ExampleApp.h"

namespace NAMESPACE{

	ExampleApp::ExampleApp(int screenWidth, int screenHeight) : AppBase(screenWidth, screenHeight) {}

	bool ExampleApp::Initialize()
	{
		if (AppBase::Initialize() == false)
			return false;

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
	}
}

