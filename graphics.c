#include "efi.h"
#include "graphics.h"
#include "common.h"
#include "file.h"

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
    struct EFI_FILE_PROTOCOL *root;
    SFSP->OpenVolume(SFSP, &root);
    struct EFI_FILE_PROTOCOL *image_file;
    root->Open(root, &image_file, L"image.bmp", EFI_FILE_MODE_READ, 0);

    struct BMPImage image;
    EFI_STATUS status;
    UINTN size;

    // ファイルヘッダの読み込み
    size = 14;
    status = image_file->Read(image_file, &size, &image.fileHeader);
    assert(status, L"fileHeader ERROR");

    // 情報ヘッダの読み込み
    size = 40;
    status = image_file->Read(image_file, &size, &image.infoHeader);
    assert(status, L"infoHeader ERROR");

    // ビット深度の確認
    assert(!(image.infoHeader.bibitcount == 24), L"24bit bitmap only");

    // 画像データのサイズを計算
    unsigned int biw = image.infoHeader.biw;
    unsigned int bih = image.infoHeader.bih;
    unsigned int row_size = ((biw * 3 + 3) / 4) * 4;
    unsigned int image_size = row_size * bih;

    status = ST->BootServices->AllocatePool(EfiLoaderData, image_size, (void **)&image.data); // 問題の箇所
    assert(status, L"AllocatePool");

    size = image_size;
    status = image_file->Read(image_file, &size, image.data);
    assert(status, L"image.data ERROR");

    unsigned int hr = GOP->Mode->Info->HorizontalResolution;
    unsigned int vr = GOP->Mode->Info->VerticalResolution;

    unsigned int x, y;
    for (y = 0; y < bih; y++)
    {
        for (x = 0; x < biw; x++)
        {
            unsigned int i = (bih - 1 - y) * row_size + x * 3;
            struct BGR *bgr = (struct BGR *)&image.data[i];
            struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL color = {bgr->red, bgr->green, bgr->blue, 0x00};
            draw_pixel(x, y, color);
        }
    }

    // メモリ開放
    status = ST->BootServices->FreePool(image.data);
    assert(status, L"FreePool");

    while (1)
        ;
}