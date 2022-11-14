#pragma once

// this will handle shader compiling in the future
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
void shadercompile(LPCSTR filename, LPCSTR funcName, LPCSTR profile, LPD3DXBUFFER* outBuffer, LPD3DXBUFFER* errBuffer)
{
	D3DXCompileShaderFromFileA(
		filename,
		NULL,
		NULL,
		funcName,
		profile,
		D3DXSHADER_AVOID_FLOW_CONTROL,
		outBuffer,
		errBuffer,
		NULL
	);
}