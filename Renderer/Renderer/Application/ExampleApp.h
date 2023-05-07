#pragma once

#include <Utils/CustomMacros.h>
#include "AppBase.h"

namespace NAMESPACE
{
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
        Graphics* graphics;
	};
}

