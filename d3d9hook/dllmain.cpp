// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <d3d9.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include <string>
#include "gms.h"
#include <iostream>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

LPD3DXFONT pFont = NULL;
LPDIRECT3DDEVICE9 pDevice = NULL;

VOID WriteText(LPDIRECT3DDEVICE9 pDevice, INT x, INT y, DWORD color, CHAR* text)
{
    RECT rect;
    SetRect(&rect, x, y, x, y);
    pFont->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP | DT_LEFT, color);
}

gmx GMBOOL initialize(stringToDLL handle)
{
    if (pDevice == NULL)
    {
        pDevice = (LPDIRECT3DDEVICE9)handle;
    }
    // Create font
    if (pFont)
    {
        pFont->Release();
        pFont = NULL;
    }
    if (!pFont)
    {
        D3DXCreateFontA(pDevice, 14, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, (LPCSTR)"Arial", &pFont);
    }
    return GMTRUE;
}

gmx GMBOOL updateHandle(stringToDLL handle)
{
    pDevice = (LPDIRECT3DDEVICE9)handle;
    return GMTRUE;
}

// Maybe theres a more efficient way to get the handle other than passing it every frame... 
// AFAIK the device handle only changes on window change, similar as to surfaces??
gmx GMBOOL drawTo( stringToDLL text, GMINT dx, GMINT dy)
{
    WriteText(pDevice, (INT)dx, (INT)80, D3DCOLOR_ARGB(255, 255, 000, 000),gmu::string_to_charptr(gmu::constcharptr_to_string(text)));      
    return GMTRUE;
}


// DLLENTRY
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        std::cout << "D3DXHook attached to process!" << std::endl;
    }
    return TRUE;
}




