#pragma once

#include <memory>

#include <source/utils/imgui/backends/imgui_impl_win32.h>
#include <source/utils/imgui/backends/imgui_impl_dx11.h>
#include <source/framework/overlay/menu.hpp>

#include <d3dx11tex.h>
#include <d3d11.h>

namespace utils
{
    class c_image
    {
    public:

        struct image
        {
            void load_image_library()
            {
                ID3D11Device* device = g_pd3dDevice;

                HRESULT hr = D3DX11CreateShaderResourceViewFromMemory()
            }
            //void load_image_search();
            //void load_image_preview();

            struct image_texture
            {
                static inline ID3D11ShaderResourceView* skeleton = nullptr; // the image

                inline ID3D11ShaderResourceView* get_current_image()
                {
                    
                    return skeleton; // example 
                }
            };



        };

    }; inline std::unique_ptr<c_image> image = std::make_unique<c_image>();
}