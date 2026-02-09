#ifndef FB_H
#define FB_H

#include "lareos.h"

#define FB_DEFAULT_WIDTH    800
#define FB_DEFAULT_HEIGHT   600
#define FB_DEFAULT_DEPTH    32

#define COLOR_BLACK         0x00000000
#define COLOR_WHITE         0x00FFFFFF
#define COLOR_RED           0x00FF0000
#define COLOR_GREEN         0x0000FF00
#define COLOR_BLUE          0x000000FF
#define COLOR_CYAN          0x0006B6D4
#define COLOR_YELLOW        0x00EAB308
#define COLOR_ORANGE        0x00F97316
#define COLOR_GRAY          0x006B7280
#define COLOR_DARK          0x000A0E17
#define COLOR_SURFACE       0x00131929
#define COLOR_ACCENT        0x003B82F6

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t depth;
    uint8_t *buffer;
    uint32_t size;
    bool initialized;
} framebuffer_t;

bool fb_init(uint32_t width, uint32_t height, uint32_t depth);
void fb_putpixel(uint32_t x, uint32_t y, uint32_t color);
void fb_fillrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void fb_clear(uint32_t color);
void fb_putchar(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg);
void fb_puts(uint32_t x, uint32_t y, const char *s, uint32_t fg, uint32_t bg);
void fb_draw_progress_bar(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t percent, uint32_t fg, uint32_t bg);
void fb_scroll_up(uint32_t lines);
framebuffer_t *fb_get_info(void);

#endif
