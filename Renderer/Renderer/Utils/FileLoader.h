#pragma once

#include <Utils/CustomMacros.h>
#include <Windows/Window.h>
#include <commdlg.h>
#include <string>
#include <vector>

#include "ModelLoader.h"

namespace NAMESPACE
{
	using std::string;
	using std::vector;

	class FileLoader
	{
	public:
		FileLoader(Window& wnd);
		FileLoader(const FileLoader&) = delete;
		FileLoader& operator=(const FileLoader&) = delete;

		bool LoadModel();

	private:
		OPENFILENAME m_openModelOFN;
		OPENFILENAME m_openTextureOFN;
		ModelLoader m_modelLoader;

		wchar_t m_openModelFileName[128];
		wchar_t m_openTextureFileName[128];
	};
}


