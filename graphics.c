#include "efi.h"
#include "graphics.h"
#include "common.h"
#include "file.h"

const struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL white = {0xff, 0xff, 0xff, 0xff};
const struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL yellow = {0x00, 0xff, 0xff, 0xff};
const struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL black = {0x00, 0x00, 0x00, 0x00};

void draw_pixel(unsigned int x, unsigned int y, struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL color)
{
    unsigned int hr = GOP->Mode->Info->HorizontalResolution;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *base = (struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)GOP->Mode->FrameBufferBase;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p = base + (hr * y) + x;
    *p = color;
}

struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL get_pixel(unsigned int x, unsigned int y)
{
    unsigned int hr = GOP->Mode->Info->HorizontalResolution;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *base = (struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)GOP->Mode->FrameBufferBase;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p = base + (hr * y) + x;
    return *p;
}

struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL interpolate(struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p1, struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p2, float t)
{
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL result;
    result.Blue = (p1.Blue * (1 - t) + p2.Blue * t);
    result.Green = (p1.Green * (1 - t) + p2.Green * t);
    result.Red = (p1.Red * (1 - t) + p2.Red * t);
    result.Reserved = 0xff;
    return result;
}

void draw_scaled_image(struct BMPImage *image, unsigned int screen_width, unsigned int screen_height)
{
    unsigned int image_width = image->infoHeader.biw;
    unsigned int image_height = image->infoHeader.bih;
    unsigned int row_size = ((image_width * 3 + 3) / 4) * 4;

    float scale_x = (float)screen_width / image_width;
    float scale_y = (float)screen_height / image_height;
    float scale = (scale_x < scale_y) ? scale_x : scale_y;

    unsigned int scaled_width = (unsigned int)(image_width * scale);
    unsigned int scaled_height = (unsigned int)(image_height * scale);

    unsigned int start_x = (screen_width - scaled_width) / 2;
    unsigned int start_y = (screen_height - scaled_height) / 2;

    for (unsigned int y = 0; y < screen_height; y++)
    {
        for (unsigned int x = 0; x < screen_width; x++)
        {
            if (x < start_x || x >= start_x + scaled_width || y < start_y || y >= start_y + scaled_height)
            {
                draw_pixel(x, y, black);
            }
            else
            {
                float src_x = (x - start_x) / scale;
                float src_y = (y - start_y) / scale;

                unsigned int src_x1 = (unsigned int)src_x;
                unsigned int src_y1 = (unsigned int)src_y;
                unsigned int src_x2 = (src_x1 + 1 < image_width) ? src_x1 + 1 : src_x1;
                unsigned int src_y2 = (src_y1 + 1 < image_height) ? src_y1 + 1 : src_y1;

                float t_x = src_x - src_x1;
                float t_y = src_y - src_y1;

                unsigned int i11 = (image_height - 1 - src_y1) * row_size + src_x1 * 3;
                unsigned int i12 = (image_height - 1 - src_y1) * row_size + src_x2 * 3;
                unsigned int i21 = (image_height - 1 - src_y2) * row_size + src_x1 * 3;
                unsigned int i22 = (image_height - 1 - src_y2) * row_size + src_x2 * 3;

                struct BGR *bgr11 = (struct BGR *)&image->data[i11];
                struct BGR *bgr12 = (struct BGR *)&image->data[i12];
                struct BGR *bgr21 = (struct BGR *)&image->data[i21];
                struct BGR *bgr22 = (struct BGR *)&image->data[i22];

                struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p11 = {bgr11->red, bgr11->green, bgr11->blue, 0xff};
                struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p12 = {bgr12->red, bgr12->green, bgr12->blue, 0xff};
                struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p21 = {bgr21->red, bgr21->green, bgr21->blue, 0xff};
                struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p22 = {bgr22->red, bgr22->green, bgr22->blue, 0xff};

                struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p1 = interpolate(p11, p12, t_x);
                struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p2 = interpolate(p21, p22, t_x);
                struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL p = interpolate(p1, p2, t_y);

                draw_pixel(x, y, p);
            }
        }
    }
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

    size = 14;
    status = image_file->Read(image_file, &size, &image.fileHeader);
    assert(status, L"fileHeader ERROR");

    size = 40;
    status = image_file->Read(image_file, &size, &image.infoHeader);
    assert(status, L"infoHeader ERROR");

    assert(!(image.infoHeader.bibitcount == 24), L"24bit bitmap only");

    unsigned int biw = image.infoHeader.biw;
    unsigned int bih = image.infoHeader.bih;
    unsigned int row_size = ((biw * 3 + 3) / 4) * 4;
    unsigned int image_size = row_size * bih;

    status = ST->BootServices->AllocatePool(EfiLoaderData, image_size, (void **)&image.data);
    assert(status, L"AllocatePool");

    size = image_size;
    status = image_file->Read(image_file, &size, image.data);
    assert(status, L"image.data ERROR");

    unsigned int screen_width = GOP->Mode->Info->HorizontalResolution;
    unsigned int screen_height = GOP->Mode->Info->VerticalResolution;

    draw_scaled_image(&image, screen_width, screen_height);

    status = ST->BootServices->FreePool(image.data);
    assert(status, L"FreePool");

    while (1)
        ;
}