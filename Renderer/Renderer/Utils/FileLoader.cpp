#include "FileLoader.h"

#include <Utils/StringEncode.h>

#pragma comment(lib, "Comdlg32.lib")

namespace NAMESPACE
{
	using std::vector;

	FileLoader::FileLoader(Window& wnd)
	{
		memset(&m_openModelOFN, 0, sizeof(OPENFILENAME));
		memset(&m_openModelFileName, 0, sizeof(m_openModelFileName));

		m_openModelOFN.lStructSize = sizeof(OPENFILENAME);
		m_openModelOFN.hwndOwner = wnd.GetWindowHandle();

		m_openModelOFN.lpstrFile = m_openModelFileName;
		m_openModelOFN.lpstrFilter = L"fbx 파일\0*.fbx";
		m_openModelOFN.nMaxFile = MAX_PATH;
		m_openModelOFN.nMaxFileTitle = MAX_PATH;

		memset(&m_openTextureOFN, 0, sizeof(OPENFILENAME));
		memset(&m_openTextureFileName, 0, sizeof(m_openTextureFileName));

		m_openTextureOFN.lStructSize = sizeof(OPENFILENAME);
		m_openTextureOFN.hwndOwner = wnd.GetWindowHandle();

		m_openTextureOFN.lpstrFile = m_openTextureFileName;
		m_openTextureOFN.lpstrFilter = L"png 파일\0*.png";
		m_openTextureOFN.nMaxFile = MAX_PATH;
		m_openTextureOFN.nMaxFileTitle = MAX_PATH;
	}

	bool FileLoader::LoadModel()
	{
		if (GetOpenFileName(&m_openModelOFN) != 0)
		{
			string filePath = WideToMultiU8(m_openModelFileName);

			m_modelLoader.Load(filePath);
			return true;
		}
		else
		{
			return false;
		}
	}

}