



#include "stdafx.h"


#include "GfxShader.h"


#include "Util/GfxCrc32.h"


using namespace GfxLib;



Shader::Shader()
	:m_pShaderByteCode(nullptr)
	,m_BytecodeLength(0)
	,m_HashValue(0)
{


}


Shader::~Shader()
{
	Finalize();
}


void	Shader::Finalize(bool delayDelete)
{
	// @TODO	delayDelete対応。一旦ID3DBlobに突っ込むほうが良いかも

	if (m_pShaderByteCode) {
		::free(const_cast<void*>(m_pShaderByteCode));
		m_pShaderByteCode = nullptr;
		m_BytecodeLength = 0;
		m_HashValue = 0;
	}
}


bool	Shader::CreateFromFile(const wchar_t * path)
{
	Finalize();


	HANDLE hFile = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE) {
		GFX_ERROR(L"CreateFile Failed... (%s)",path);
		return false;
	}

	size_t FileSize = ::GetFileSize(hFile,nullptr);

	if (FileSize == 0) {
		GFX_ERROR(L"Zero file size... (%s)", path);
		::CloseHandle(hFile);
		return false;
	}

	void *ptr = ::malloc(FileSize);
	if (!ptr) {
		GFX_ERROR(L"malloc failed... (%s)", path);
		::CloseHandle(hFile);

		return false;
	}


	::ReadFile(hFile, ptr, (DWORD)FileSize, nullptr, nullptr);
	::CloseHandle(hFile);

	m_pShaderByteCode = ptr;
	m_BytecodeLength = FileSize;
	

	//	ハッシュ値の計算
	Crc32 crc;
	crc.Update(m_pShaderByteCode, m_BytecodeLength);

	m_HashValue = crc.GetValue();


	return true;

}











