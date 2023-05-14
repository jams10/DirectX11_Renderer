#pragma once

#include <Utils/CustomMacros.h>


namespace NAMESPACE
{
	class SceneBase
	{
	public:
		virtual bool Initialize() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() = 0;

	public:
		bool hasInitialized;
	};
}


