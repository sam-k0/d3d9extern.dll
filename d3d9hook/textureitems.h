#pragma once

#include <d3d9.h>
#include <d3dx9.h>

struct TextureItems
{
    LPDIRECT3DTEXTURE9 imagetex = nullptr; //textute our image will be loaded into
    LPD3DXSPRITE sprite = nullptr; //sprite to display our image
};

namespace ti
{
    bool ok(TextureItems* t)
    {
        return ((t->imagetex != nullptr) && (t->sprite != nullptr) && true);
    }
}