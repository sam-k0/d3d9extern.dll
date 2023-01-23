#pragma once
#include <Vfw.h>
#include <string>
#include <iostream>
#include <d3d9.h>
#include <d3d9helper.h>
#include <d3dx9core.h>

struct aviVertex{  float x, y, z, rhw;  float u, v;};
#define D3DFVF_AVIVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)

class CAviTexture{

	protected:  PAVISTREAM    m_pavi;
    AVISTREAMINFO m_si;
    PGETFRAME     m_pgf;
    int m_width, m_height; 
    long m_lastFrame;  
    int m_mpf;  
    BITMAPINFOHEADER m_bmih; 
    HDC m_hDC;  
    HBITMAP m_hBitmap; 
    unsigned short* m_pData; 
    HDRAWDIB m_hdd;  
    LPDIRECT3DTEXTURE9 m_pTexture;
    int m_next; 
    int m_frame;

    public:  
        LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; } 
        void SetTexture(LPDIRECT3DTEXTURE9 tex) { m_pTexture = tex; }

        void Open(LPDIRECT3DDEVICE9 pDevice, LPCSTR szFile);
        void Close();  
        void Update(float frameTime); 
        void SetTexture(LPDIRECT3DDEVICE9 pDevice); 
        bool Draw(LPDIRECT3DDEVICE9 pDevice, float x, float y, float width, float height);
        bool Draw(LPD3DXSPRITE pSprite, float x, float y);
};

