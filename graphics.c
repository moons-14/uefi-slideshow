#include "efi.h"
#include "graphics.h"
#include "common.h"
#include "file.h"

const struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL black = {0x00, 0x00, 0x00, 0x00};

void draw_pixel(unsigned int x, unsigned int y, struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL color)
{
    unsigned int hr = GOP->Mode->Info->HorizontalResolution;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *base = (struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)GOP->Mode->FrameBufferBase;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p = base + (hr * y) + x;
    *p = color;
}

struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL get_pixel_from_image(struct BMPImage *image, unsigned int x, unsigned int y)
{
    unsigned int row_size = ((image->infoHeader.biw * 3 + 3) / 4) * 4;
    unsigned int i = (image->infoHeader.bih - 1 - y) * row_size + x * 3;
    struct BGR *bgr = (struct BGR *)&image->data[i];
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL color = {bgr->red, bgr->green, bgr->blue, 0xff};
    return color;
}

void draw_scaled_image(struct BMPImage *image, unsigned int screen_width, unsigned int screen_height)
{
    unsigned int image_width = image->infoHeader.biw;
    unsigned int image_height = image->infoHeader.bih;

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
                unsigned int src_x = (unsigned int)((x - start_x) / scale);
                unsigned int src_y = (unsigned int)((y - start_y) / scale);

                struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL color = get_pixel_from_image(image, src_x, src_y);
                draw_pixel(x, y, color);
            }
        }
    }
}

EFI_STATUS load_bmp_image(struct EFI_FILE_PROTOCOL *file, struct BMPImage *image)
{
    EFI_STATUS status;
    UINTN size;

    // Read file header
    size = 14;
    status = file->Read(file, &size, &image->fileHeader);
    if (EFI_ERROR(status))
        return status;

    // Read info header
    size = 40;
    status = file->Read(file, &size, &image->infoHeader);
    if (EFI_ERROR(status))
        return status;

    // Check if it's a 24-bit bitmap
    if (image->infoHeader.bibitcount != 24)
    {
        return EFI_UNSUPPORTED;
    }

    // Calculate image size and allocate memory
    unsigned int row_size = ((image->infoHeader.biw * 3 + 3) / 4) * 4;
    unsigned int image_size = row_size * image->infoHeader.bih;

    status = ST->BootServices->AllocatePool(EfiLoaderData, image_size, (void **)&image->data);
    if (EFI_ERROR(status))
        return status;

    // Read image data
    size = image_size;
    status = file->Read(file, &size, image->data);
    if (EFI_ERROR(status))
    {
        ST->BootServices->FreePool(image->data);
        return status;
    }

    return EFI_SUCCESS;
}

void draw(void)
{
    struct EFI_FILE_PROTOCOL *root;
    EFI_STATUS status = SFSP->OpenVolume(SFSP, &root);
    if (EFI_ERROR(status))
    {
        putc(L"Failed to open root volume\n");
        return;
    }

    struct EFI_FILE_PROTOCOL *image_file;
    status = root->Open(root, &image_file, L"image.bmp", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status))
    {
        putc(L"Failed to open image file\n");
        return;
    }

    struct BMPImage image;
    status = load_bmp_image(image_file, &image);
    if (EFI_ERROR(status))
    {
        putc(L"Failed to load BMP image\n");
        image_file->Close(image_file);
        return;
    }

    unsigned int screen_width = GOP->Mode->Info->HorizontalResolution;
    unsigned int screen_height = GOP->Mode->Info->VerticalResolution;

    draw_scaled_image(&image, screen_width, screen_height);

    status = ST->BootServices->FreePool(image.data);
    if (EFI_ERROR(status))
    {
        putc(L"Failed to free image data\n");
    }

    image_file->Close(image_file);
}