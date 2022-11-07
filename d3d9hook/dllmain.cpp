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
#include "mybitmaps.h"
#include <vector>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

struct TextureItems
{
    LPDIRECT3DTEXTURE9 imagetex = nullptr; //textute our image will be loaded into
    LPD3DXSPRITE sprite = nullptr; //sprite to display our image
};

LPD3DXFONT pFont = NULL;
LPDIRECT3DDEVICE9 pDevice = NULL; // The Window Device handle
TextureItems myTexture; // Daniel Jung

std::vector<TextureItems*> textures;

/* DX functions */
VOID WriteText(LPDIRECT3DDEVICE9 pDevice, INT x, INT y, DWORD color, CHAR* text)
{
    RECT rect;
    SetRect(&rect, x, y, x, y);
    pFont->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP | DT_LEFT, color);
}

// https://guidedhacking.com/threads/how-to-draw-images-with-directx.17645/
void DrawTexture(int x, int y, LPDIRECT3DTEXTURE9 dTexture, LPD3DXSPRITE sprite)
{
    // Allow use of alpha values
    sprite->Begin(D3DXSPRITE_ALPHABLEND);

    D3DXVECTOR3 pos = D3DXVECTOR3(x, y, 0.0f);
    sprite->Draw(dTexture, NULL, NULL, &pos, 0xFFFFFFFF);

    sprite->End();
}

/*
    Draws a textures specified by textureItems
*/
void DrawImage(int x, int y, TextureItems* textureItems)
{
    if (!pDevice || !textureItems)
        return;

    if (textureItems->imagetex == nullptr || textureItems->sprite == nullptr)
        return;

    DrawTexture(x, y, textureItems->imagetex, textureItems->sprite);
}

/*
    Adds an image from given filepath to the vector of TexureItems
*/
int AddImageFromFile(LPCSTR filename)
{
    TextureItems* tex = new TextureItems();

    if (FAILED(D3DXCreateTextureFromFileA(pDevice, filename, &tex->imagetex)))
    {
        delete tex;
        return -1;
    }
       

    if (FAILED(D3DXCreateSprite(pDevice, &tex->sprite)))
    {
        delete tex;
        return -1;
    }


    textures.push_back(tex);
    return textures.size() - 1;
}

int FreeTexture(int pos)
{
    textures.at(pos)->imagetex->Release();
    textures.at(pos)->sprite->Release();
    return S_OK;
}

// DLLEXPORT

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

gmx GMBOOL dx9_load_from_mem() // Unused pls, switch to adding them to the vec
{
    if (FAILED(D3DXCreateTextureFromFileInMemory(pDevice, &bmDaniel, sizeof(bmDaniel), &myTexture.imagetex)))
        std::cout << "D3DXCreateTextureFromFileInMemory failed" << std::endl;

    if (FAILED(D3DXCreateSprite(pDevice, &myTexture.sprite)))
        std::cout << "D3DXCreateSprite failed" << std::endl;
    return GMTRUE;
}

gmx GMINT dx9_load_image(stringToDLL path)
{
    return GMINT(AddImageFromFile(path));
}

gmx GMBOOL dx9_set_handle(stringToDLL handle)
{
    pDevice = (LPDIRECT3DDEVICE9)handle;
    return GMTRUE;
}

// Maybe theres a more efficient way to get the handle other than passing it every frame... 
// AFAIK the device handle only changes on window change, similar as to surfaces??
gmx GMBOOL dx9_draw_text( stringToDLL text, GMINT dx, GMINT dy)
{
    WriteText(pDevice, (INT)dx, (INT)80, D3DCOLOR_ARGB(255, 255, 000, 000),gmu::string_to_charptr(gmu::constcharptr_to_string(text)));      
    return GMTRUE;
}

gmx GMBOOL dx9_draw_image(GMINT dx, GMINT dy, GMINT imageid)
{
    if (imageid < 0) return GMFALSE; // TODO: More err catching

    TextureItems* ti;
    ti = textures.at(int(imageid));
    DrawImage(dx, dy, ti);
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




