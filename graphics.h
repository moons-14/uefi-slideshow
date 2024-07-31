#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "efi.h"

extern const struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL white;
extern const struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL yellow;

void draw_pixel(unsigned int x, unsigned int y, struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL color);
struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL get_pixel(unsigned int x, unsigned int y);
void draw(void);

#endif