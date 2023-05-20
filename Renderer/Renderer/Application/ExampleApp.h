#pragma once

#include <vector>

#include <Utils/CustomMacros.h>
#include <Model/ModelData.h>
#include <Scene/SceneBase.h>

#include "AppBase.h"

namespace NAMESPACE
{
    using std::vector;

	class ExampleApp : public AppBase
	{
    public:
        ExampleApp(int screenWidth, int screenHeight);
        ExampleApp(const ExampleApp&) = delete;
        ExampleApp& operator=(const ExampleApp&) = delete;

        virtual bool Initialize() override;
        virtual void UpdateGUI() override;
        virtual void Update(float dt) override;
        virtual void Render() override;

    private:
        /*
        *   UI
        */
        bool m_openSystemInfoWindow = false;
        bool m_openSceneSelectWindow = false;

        SceneBase* currentScene;
	};
}

