#include "efi.h"
#include "graphics.h"
#include "common.h"

const struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL white = {0xff, 0xff, 0xff, 0xff};
const struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL yellow = {0x00, 0xff, 0xff, 0xff};

void draw_pixel(unsigned int x, unsigned int y, struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL color)
{
    unsigned int hr = GOP->Mode->Info->HorizontalResolution; // 水平解像度

    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *base = (struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)GOP->Mode->FrameBufferBase; // フレームバッファの先頭アドレス

    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p = base + (hr * y) + x; // 描画するピクセルのアドレス

    p->Blue = color.Blue;
    p->Green = color.Green;
    p->Red = color.Red;
    p->Reserved = color.Reserved;
}


struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL get_pixel(unsigned int x, unsigned int y)
{
    unsigned int hr = GOP->Mode->Info->HorizontalResolution; // 水平解像度

    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *base = (struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)GOP->Mode->FrameBufferBase; // フレームバッファの先頭アドレス

    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p = base + (hr * y) + x; // 描画するピクセルのアドレス

    return *p;
}

void draw(void)
{
    // GOPを使って前面を黄色で塗りつぶす
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL color = {0x00, 0xff, 0xff, 0x00};
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *base = (struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)GOP->Mode->FrameBufferBase;

    for (unsigned  y = 0; y < GOP->Mode->Info->VerticalResolution; y++)
    {
        for (unsigned  int x = 0; x < GOP->Mode->Info->HorizontalResolution; x++)
        {
            struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p = base + y * GOP->Mode->Info->HorizontalResolution + x;
            p->Blue = color.Blue;
            p->Green = color.Green;
            p->Red = color.Red;
            p->Reserved = color.Reserved;
        }
    }

    while (1)
        ;
}