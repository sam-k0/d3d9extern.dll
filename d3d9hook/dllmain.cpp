// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "textureitems.h"
#include "vertexformat.h"

#include "YYAviPlayer.h"

//#include <d3d9.h>
#include <windows.h>
#include <d3d9.h>

#include <stdio.h>
#include <string>
#include "gms.h"
#include <iostream>
#include "mybitmaps.h"
#include <vector>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")



// DLL-global scope variables
LPD3DXFONT pFont = NULL;
LPDIRECT3DDEVICE9 pDevice = NULL; // The Window Device handle
TextureItems myTexture; // Daniel Jung
LPDIRECT3DTEXTURE9 AviTexture = NULL;
LPD3DXSPRITE AviSprite = NULL;

std::vector<TextureItems*> textures; // stores all texture items

// AVI player
CAviTexture* aviPlayer = nullptr;


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

bool FreeTexture(int pos)
{
    TextureItems* ti = textures.at(pos);
    if (!ti::ok(ti))
    {
        return false;
    }

    if (FAILED(ti->imagetex->Release()) || FAILED(ti->sprite->Release()))
    {
        return false;
    }
    textures.at(pos)->imagetex = nullptr;
    textures.at(pos)->sprite = nullptr;

    return true;
}

// DLLEXPORT

gmx GMBOOL dx9_initialize(stringToDLL handle)
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

// unused
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

gmx GMBOOL dx9_free_image(GMINT imageid)
{
    return GMBOOL(FreeTexture(imageid));
}

// Performance testing 
// Draws an image 1.000.000 times
gmx GMBOOL dx9_masstest(GMINT dx, GMINT dy, GMINT imageid)
{
    for (int i = 0; i < 1000000; i++)
    {
        dx9_draw_image(dx, dy, imageid);
    }
    
    return GMTRUE;
}

// Other tests

gmx GMBOOL dx9_draw_triangle()
{
    CUSTOMVERTEX triangle[] =
    {
        {320.0f, 50.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(0,0,255)},
        {520.0f, 400.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(0,255,0)},
        {120.0f, 400.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255,0,0)}
    };

    LPDIRECT3DVERTEXBUFFER9 vbuf;

    pDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &vbuf,
        NULL);
    
    VOID* pVoid;

    vbuf->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, triangle, sizeof(triangle));
    vbuf->Unlock();

    pDevice->SetFVF(CUSTOMFVF);
    pDevice->SetStreamSource(0, vbuf, 0, sizeof(CUSTOMVERTEX));
    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    return GMTRUE;
}

// Avi player

gmx GMBOOL avi_init(stringToDLL path)
{
    if (aviPlayer)return GMFALSE;

    if (FAILED(pDevice->CreateTexture(512, 512, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &AviTexture, NULL)))
    {
        cout << "Failed to create tex" << endl;
    }
    else
    {
        cout << "Success create tex" << endl;
    }

    if (FAILED(D3DXCreateSprite(pDevice, &AviSprite)))
    {
        gmu::debugmessage("Failed to create Sprite");
    }

    aviPlayer = new CAviTexture();

    path = "C:\\Users\\Samuel\\Documents\\GameMaker\\Projects\\YYVideoPlayerGMX.gmx\\datafiles\\test.avi";

    cout << "Trying to open aviplayer with path " << path << endl;


    aviPlayer->SetTexture(AviTexture);


    aviPlayer->Open(pDevice , path );

    return GMTRUE;
}

gmx GMBOOL avi_update(GMINT passedMillis)
{
    cout << "Updating.." << endl;
    aviPlayer->Update(passedMillis);
    cout << "post update" << endl;
    return GMTRUE;
}

gmx GMBOOL avi_draw(GMINT x, GMINT y, GMINT w, GMINT h)
{
    cout << "draw!" << endl;

    gmu::debugmessage("Set the device texture!");
    //viPlayer->SetTexture(pDevice);
    
    gmu::debugmessage("Drawing the texture!");
    aviPlayer->Draw(pDevice, x, y, w, h);
  
    // Allow use of alpha values
    AviSprite->Begin(D3DXSPRITE_ALPHABLEND);

    D3DXVECTOR3 pos = D3DXVECTOR3(x, y, 0.0f);
    AviSprite->Draw(AviTexture, NULL, NULL, &pos, 0xFFFFFFFF);

    AviSprite->End();

    cout << "post draw" << endl;
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




