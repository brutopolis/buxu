// wrapper for tigr library check it out at https://github.com/erkkah/tigr

#include <buxu.h>
#include "tigr/tigr.h"

function(bu_tigr_window)
{
    Int result = new_var(vm);
    Int width = arg(0).number;
    Int height = arg(1).number;
    char* title = arg(2).string;
    Int flags = arg(3).number;
    Tigr *bmp = tigrWindow(width, height, title, flags);
    data(result).pointer = bmp;
    return result;
}

function(bu_tigr_bitmap)
{
    Int result = new_var(vm);
    Int width = arg(0).number;
    Int height = arg(1).number;
    Tigr *bmp = tigrBitmap(width, height);
    data(result).pointer = bmp;
    return result;
}

function(bu_tigr_free)
{
    tigrFree(arg(0).pointer);
    unuse_var(vm, arg_i(0));
    return -1;
}

function(bu_tigr_closed)
{
    Tigr *bmp = arg(0).pointer;
    return tigrClosed(bmp);
}

function(bu_tigr_update)
{
    Tigr *bmp = arg(0).pointer;
    tigrUpdate(bmp);
    return -1;
}

function(bu_tigr_begin_opengl)
{
    Tigr *bmp = arg(0).pointer;
    return tigrBeginOpenGL(bmp);
}

function(bu_tigr_set_post_shader)
{
    Tigr *bmp = arg(0).pointer;
    char* code = arg(1).string;
    int size = arg(2).number;
    tigrSetPostShader(bmp, code, size);
    return -1;
}

function(bu_tigr_set_post_fx)
{
    Tigr *bmp = arg(0).pointer;
    float p1 = arg(1).number;
    float p2 = arg(2).number;
    float p3 = arg(3).number;
    float p4 = arg(4).number;
    tigrSetPostFX(bmp, p1, p2, p3, p4);
    return -1;
}

function(bu_tigr_get)
{
    Tigr *bmp = arg(0).pointer;
    int x = arg(1).number;
    int y = arg(2).number;
    TPixel pix = tigrGet(bmp, x, y);
    Int result = new_var(vm);
    data(result).u8[0] = pix.r;
    data(result).u8[1] = pix.g;
    data(result).u8[2] = pix.b;
    data(result).u8[3] = pix.a;
    return result;
}

function(bu_tigr_plot)
{
    Tigr *bmp = arg(0).pointer;
    int x = arg(1).number;
    int y = arg(2).number;

    TPixel pix;
    pix.r = arg(3).u8[0];
    pix.g = arg(3).u8[1];
    pix.b = arg(3).u8[2];
    pix.a = arg(3).u8[3];

    tigrPlot(bmp, x, y, pix);
    return -1;
}

function(bu_tigr_clear)
{
    Tigr *bmp = arg(0).pointer;
    TPixel pix;
    pix.r = arg(1).u8[0];
    pix.g = arg(1).u8[1];
    pix.b = arg(1).u8[2];
    pix.a = arg(1).u8[3];
    tigrClear(bmp, pix);
    return -1;
}

function(bu_tigr_fill)
{
    Tigr *bmp = arg(0).pointer;
    int x = arg(1).number;
    int y = arg(2).number;
    int w = arg(3).number;
    int h = arg(4).number;
    TPixel pix;
    pix.r = arg(5).u8[0];
    pix.g = arg(5).u8[1];
    pix.b = arg(5).u8[2];
    pix.a = arg(5).u8[3];

    tigrFill(bmp, x, y, w, h, pix);
    return -1;
}

function(bu_tigr_line)
{
    Tigr *bmp = arg(0).pointer;
    int x0 = arg(1).number;
    int y0 = arg(2).number;
    int x1 = arg(3).number;
    int y1 = arg(4).number;

    TPixel pix;
    pix.r = arg(5).u8[0];
    pix.g = arg(5).u8[1];
    pix.b = arg(5).u8[2];
    pix.a = arg(5).u8[3];

    tigrLine(bmp, x0, y0, x1, y1, pix);
    return -1;
}

function(bu_tigr_rect)
{
    Tigr *bmp = arg(0).pointer;
    int x = arg(1).number;
    int y = arg(2).number;
    int w = arg(3).number;
    int h = arg(4).number;

    TPixel pix;
    pix.r = arg(5).u8[0];
    pix.g = arg(5).u8[1];
    pix.b = arg(5).u8[2];
    pix.a = arg(5).u8[3];

    tigrRect(bmp, x, y, w, h, pix);
    return -1;
}

function(bu_tigr_fill_rect)
{
    Tigr *bmp = arg(0).pointer;
    int x = arg(1).number;
    int y = arg(2).number;
    int w = arg(3).number;
    int h = arg(4).number;

    TPixel pix;
    pix.r = arg(5).u8[0];
    pix.g = arg(5).u8[1];
    pix.b = arg(5).u8[2];
    pix.a = arg(5).u8[3];

    tigrFillRect(bmp, x, y, w, h, pix);
    return -1;
}

function(bu_tigr_circle)
{
    Tigr *bmp = arg(0).pointer;
    int x = arg(1).number;
    int y = arg(2).number;
    int r = arg(3).number;

    TPixel pix;
    pix.r = arg(4).u8[0];
    pix.g = arg(4).u8[1];
    pix.b = arg(4).u8[2];
    pix.a = arg(4).u8[3];

    tigrCircle(bmp, x, y, r, pix);
    return -1;
}

function(bu_tigr_fill_circle)
{
    Tigr *bmp = arg(0).pointer;
    int x = arg(1).number;
    int y = arg(2).number;
    int r = arg(3).number;
    TPixel pix;
    pix.r = arg(4).u8[0];
    pix.g = arg(4).u8[1];
    pix.b = arg(4).u8[2];
    pix.a = arg(4).u8[3];

    tigrFillCircle(bmp, x, y, r, pix);
    return -1;
}

function(bu_tigr_clip)
{
    Tigr *bmp = arg(0).pointer;
    int cx = arg(1).number;
    int cy = arg(2).number;
    int cw = arg(3).number;
    int ch = arg(4).number;

    tigrClip(bmp, cx, cy, cw, ch);
    return -1;
}

function(bu_tigr_blit)
{
    Tigr *dest = arg(0).pointer;
    Tigr *src = arg(1).pointer;
    int dx = arg(2).number;
    int dy = arg(3).number;
    int sx = arg(4).number;
    int sy = arg(5).number;
    int w = arg(6).number;
    int h = arg(7).number;
    tigrBlit(dest, src, dx, dy, sx, sy, w, h);
    return -1;
}

function(bu_tigr_blit_alpha)
{
    Tigr *dest = arg(0).pointer;
    Tigr *src = arg(1).pointer;
    int dx = arg(2).number;
    int dy = arg(3).number;
    int sx = arg(4).number;
    int sy = arg(5).number;
    int w = arg(6).number;
    int h = arg(7).number;
    float alpha = arg(8).number;
    tigrBlitAlpha(dest, src, dx, dy, sx, sy, w, h, alpha);
    return -1;
}

function(bu_tigr_blit_tint)
{
    Tigr *dest = arg(0).pointer;
    Tigr *src = arg(1).pointer;
    int dx = arg(2).number;
    int dy = arg(3).number;
    int sx = arg(4).number;
    int sy = arg(5).number;
    int w = arg(6).number;
    int h = arg(7).number;

    TPixel tint;
    tint.r = arg(8).u8[0];
    tint.g = arg(8).u8[1];
    tint.b = arg(8).u8[2];
    tint.a = arg(8).u8[3];

    tigrBlitTint(dest, src, dx, dy, sx, sy, w, h, tint);
    return -1;
}

function(bu_tigr_blit_mode)
{
    Tigr *dest = arg(0).pointer;
    int mode = arg(1).number;
    tigrBlitMode(dest, mode);
    return -1;
}

function(bu_tigr_rgb)
{
    Int result = new_var(vm);
    data(result).u8[0] = arg(0).number;
    data(result).u8[1] = arg(1).number;
    data(result).u8[2] = arg(2).number;
    data(result).u8[3] = 255;
    return result;
}

function(bu_tigr_rgba)
{
    Int result = new_var(vm);
    data(result).u8[0] = arg(0).number;
    data(result).u8[1] = arg(1).number;
    data(result).u8[2] = arg(2).number;
    data(result).u8[3] = arg(3).number;
    return result;
}

function(bu_tigr_load_font)
{
    Tigr *bitmap = arg(0).pointer;
    int codepage = arg(1).number;
    TigrFont *font = tigrLoadFont(bitmap, codepage);
    Int result = new_var(vm);
    data(result).pointer = font;
    return result;
}

function(bu_tigr_free_font)
{
    TigrFont *font = arg(0).pointer;
    tigrFreeFont(font);
    unuse_var(vm, arg_i(0));
    return -1;
}

function(bu_tigr_print) // no endless arguments for now
{
    Tigr *dest = arg(0).pointer;
    TigrFont *font = arg(1).pointer;
    int x = arg(2).number;
    int y = arg(3).number;

    TPixel color;
    color.r = arg(4).u8[0];
    color.g = arg(4).u8[1];
    color.b = arg(4).u8[2];
    color.a = arg(4).u8[3];
    char* text = arg(5).string;
    tigrPrint(dest, font, x, y, color, text);
    return -1;
}

function(bu_tigr_text_width)
{
    TigrFont *font = arg(0).pointer;
    char* text = arg(1).string;
    return new_number(vm, tigrTextWidth(font, text));
}

function(bu_tigr_text_height)
{
    TigrFont *font = arg(0).pointer;
    char* text = arg(1).string;
    return new_number(vm, tigrTextHeight(font, text));
}

function(bu_tigr_mouse)
{
    Tigr *bmp = arg(0).pointer;

    tigrMouse(bmp, &arg(1).i32[0], &arg(2).i32[0], &arg(3).i32[0]);

    return -1;
}

function(bu_tigr_touch) // need to be fixed
{
    /*Tigr *bmp = arg(0).pointer;
    int maxPoints = arg(1).number;

    Int result = new_array(vm, 10);

    TigrTouchPoint points[maxPoints];
    int n = tigrTouch(bmp, points, maxPoints);

    for (int i = 0; i < n; i++)
    {
        data(result+i+1).u16[0] = points[i].x;
        data(result+i+1).u16[1] = points[i].y;
    }

    return result;*/
}

function(bu_tigr_key_down)
{
    Tigr *bmp = arg(0).pointer;
    int key = arg(1).number;
    return tigrKeyDown(bmp, key);
}

function(bu_tigr_key_held)
{
    Tigr *bmp = arg(0).pointer;
    int key = arg(1).number;
    return tigrKeyHeld(bmp, key);
}

function(bu_tigr_read_char)
{
    Tigr *bmp = arg(0).pointer;
    return tigrReadChar(bmp);
}

function(bu_tigr_show_keyboard)
{
    int show = arg(0).number;
    tigrShowKeyboard(show);
    return -1;
}

function(bu_tigr_load_image)
{
    char* filename = arg(0).string;
    Tigr *bmp = tigrLoadImage(filename);
    Int result = new_var(vm);
    data(result).pointer = bmp;
    return result;
}

function(bu_tigr_load_image_mem)
{
    void* data = arg(0).pointer;
    int length = arg(1).number;
    Tigr *bmp = tigrLoadImageMem(data, length);
    Int result = new_var(vm);
    data(result).pointer = bmp;
    return result;
}

function(bu_tigr_save_image)
{
    char* filename = arg(0).string;
    Tigr *bmp = arg(1).pointer;
    return new_number(vm, tigrSaveImage(filename, bmp));
}

function(bu_tigr_time)
{
    return new_number(vm, tigrTime());
}

function(bu_tigr_error)
{
    Tigr *bmp = arg(0).pointer;
    char* message = arg(1).string;
    tigrError(bmp, message);
    return -1;
}

function(bu_tigr_read_file)
{
    char* filename = arg(0).string;
    int length;
    void* data = tigrReadFile(filename, &length);
    Int result = new_var(vm);
    data(result).pointer = data;
    return result;
}

function(bu_tigr_inflate)
{
    void* data = arg(0).pointer;
    int length = arg(1).number;
    void* buffer = arg(2).pointer;
    int size = arg(3).number;
    return new_number(vm, tigrInflate(data, length, buffer, size));
}

function(bu_tigr_decode_utf8) // might not work properly
{
    char* text = arg(0).string;
    int codepoint;
    Int result = new_var(vm);
    data(result).string = str_duplicate(tigrDecodeUTF8(text, &codepoint));
    data_t(result) = TYPE_STRING;
    return result;
}

function(bu_tigr_encode_utf8) // might not work properly
{
    char* text = arg(0).string;
    int codepoint = arg(1).number;
    Int result = new_var(vm);
    data(result).string = str_duplicate(tigrEncodeUTF8(text, codepoint));
    data_t(result) = TYPE_STRING;
    return result;
}

init(tigr)
{
    // conditional this still creating new variable for each run instead returning 0 or 1 index
    // will be fixed in the future

    //register flags
    register_number(vm, "tigr.fixed", TIGR_FIXED);
    register_number(vm, "tigr.auto", TIGR_AUTO);
    register_number(vm, "tigr.2x", TIGR_2X);
    register_number(vm, "tigr.3x", TIGR_3X);
    register_number(vm, "tigr.4x", TIGR_4X);
    register_number(vm, "tigr.retina", TIGR_RETINA);
    register_number(vm, "tigr.nocursor", TIGR_NOCURSOR);
    register_number(vm, "tigr.fullscreen", TIGR_FULLSCREEN);

    //register blit modes
    register_number(vm, "tigr.keep_alpha", TIGR_KEEP_ALPHA);
    register_number(vm, "tigr.blend_alpha", TIGR_BLEND_ALPHA);

    //register codepage
    register_number(vm, "tigr.tcp_ascii", TCP_ASCII);
    register_number(vm, "tigr.tcp_1252", TCP_1252);
    register_number(vm, "tigr.tcp_utf32", TCP_UTF32);

    //register keys
    register_number(vm, "tigr.pad0", TK_PAD0);
    register_number(vm, "tigr.pad1", TK_PAD1);
    register_number(vm, "tigr.pad2", TK_PAD2);
    register_number(vm, "tigr.pad3", TK_PAD3);
    register_number(vm, "tigr.pad4", TK_PAD4);
    register_number(vm, "tigr.pad5", TK_PAD5);
    register_number(vm, "tigr.pad6", TK_PAD6);
    register_number(vm, "tigr.pad7", TK_PAD7);
    register_number(vm, "tigr.pad8", TK_PAD8);
    register_number(vm, "tigr.pad9", TK_PAD9);
    register_number(vm, "tigr.pad_add", TK_PADADD);
    register_number(vm, "tigr.pad_sub", TK_PADSUB);
    register_number(vm, "tigr.pad_mul", TK_PADMUL);
    register_number(vm, "tigr.pad_div", TK_PADDIV);
    register_number(vm, "tigr.pad_enter", TK_PADENTER);
    register_number(vm, "tigr.pad_dot", TK_PADDOT);

    register_number(vm, "tigr.f1", TK_F1);
    register_number(vm, "tigr.f2", TK_F2);
    register_number(vm, "tigr.f3", TK_F3);
    register_number(vm, "tigr.f4", TK_F4);
    register_number(vm, "tigr.f5", TK_F5);
    register_number(vm, "tigr.f6", TK_F6);
    register_number(vm, "tigr.f7", TK_F7);
    register_number(vm, "tigr.f8", TK_F8);
    register_number(vm, "tigr.f9", TK_F9);
    register_number(vm, "tigr.f10", TK_F10);
    register_number(vm, "tigr.f11", TK_F11);
    register_number(vm, "tigr.f12", TK_F12);

    register_number(vm, "tigr.backspace", TK_BACKSPACE);
    register_number(vm, "tigr.tab", TK_TAB);
    register_number(vm, "tigr.return", TK_RETURN);
    register_number(vm, "tigr.shift", TK_SHIFT);
    register_number(vm, "tigr.control", TK_CONTROL);
    register_number(vm, "tigr.alt", TK_ALT);
    register_number(vm, "tigr.pause", TK_PAUSE);
    register_number(vm, "tigr.capslock", TK_CAPSLOCK);
    register_number(vm, "tigr.escape", TK_ESCAPE);
    register_number(vm, "tigr.space", TK_SPACE);
    
    register_number(vm, "tigr.pageup", TK_PAGEUP);
    register_number(vm, "tigr.pagedn", TK_PAGEDN);
    register_number(vm, "tigr.end", TK_END);
    register_number(vm, "tigr.home", TK_HOME);
    
    register_number(vm, "tigr.left", TK_LEFT);
    register_number(vm, "tigr.up", TK_UP);
    register_number(vm, "tigr.right", TK_RIGHT);
    register_number(vm, "tigr.down", TK_DOWN);

    register_number(vm, "tigr.insert", TK_INSERT);
    register_number(vm, "tigr.delete", TK_DELETE);
    register_number(vm, "tigr.lwin", TK_LWIN);
    register_number(vm, "tigr.rwin", TK_RWIN);
    register_number(vm, "tigr.numlock", TK_NUMLOCK);
    register_number(vm, "tigr.scroll", TK_SCROLL);
    register_number(vm, "tigr.lshift", TK_LSHIFT);
    register_number(vm, "tigr.rshift", TK_RSHIFT);
    register_number(vm, "tigr.lctrl", TK_LCONTROL);
    register_number(vm, "tigr.rctrl", TK_RCONTROL);
    register_number(vm, "tigr.lalt", TK_LALT);
    register_number(vm, "tigr.ralt", TK_RALT);
    register_number(vm, "tigr.semicolon", TK_SEMICOLON);
    register_number(vm, "tigr.equals", TK_EQUALS);
    register_number(vm, "tigr.comma", TK_COMMA);
    register_number(vm, "tigr.minus", TK_MINUS);

    register_number(vm, "tigr.dot", TK_DOT);
    register_number(vm, "tigr.slash", TK_SLASH);
    register_number(vm, "tigr.backtick", TK_BACKTICK);
    register_number(vm, "tigr.lsquare", TK_LSQUARE);
    register_number(vm, "tigr.backslash", TK_BACKSLASH);
    register_number(vm, "tigr.rsquare", TK_RSQUARE);
    register_number(vm, "tigr.tick", TK_TICK);

    //register functions
    register_builtin(vm, "tigr.window", bu_tigr_window);
    register_builtin(vm, "tigr.bitmap", bu_tigr_bitmap);
    register_builtin(vm, "tigr.free", bu_tigr_free);
    register_builtin(vm, "tigr.closed", bu_tigr_closed);
    register_builtin(vm, "tigr.update", bu_tigr_update);
    register_builtin(vm, "tigr.begin_opengl", bu_tigr_begin_opengl);
    register_builtin(vm, "tigr.set_post_shader", bu_tigr_set_post_shader);
    register_builtin(vm, "tigr.set_post_fx", bu_tigr_set_post_fx);
    register_builtin(vm, "tigr.get", bu_tigr_get);
    register_builtin(vm, "tigr.plot", bu_tigr_plot);
    register_builtin(vm, "tigr.clear", bu_tigr_clear);
    register_builtin(vm, "tigr.fill", bu_tigr_fill);
    register_builtin(vm, "tigr.line", bu_tigr_line);
    register_builtin(vm, "tigr.rect", bu_tigr_rect);
    register_builtin(vm, "tigr.fill_rect", bu_tigr_fill_rect);
    register_builtin(vm, "tigr.circle", bu_tigr_circle);
    register_builtin(vm, "tigr.fill_circle", bu_tigr_fill_circle);
    register_builtin(vm, "tigr.clip", bu_tigr_clip);
    register_builtin(vm, "tigr.blit", bu_tigr_blit);
    register_builtin(vm, "tigr.blit_alpha", bu_tigr_blit_alpha);
    register_builtin(vm, "tigr.blit_tint", bu_tigr_blit_tint);
    register_builtin(vm, "tigr.blit_mode", bu_tigr_blit_mode);

    register_builtin(vm, "tigr.load_font", bu_tigr_load_font);
    register_builtin(vm, "tigr.free_font", bu_tigr_free_font);
    register_builtin(vm, "tigr.print", bu_tigr_print);
    register_builtin(vm, "tigr.text_width", bu_tigr_text_width);
    register_builtin(vm, "tigr.text_height", bu_tigr_text_height);

    register_builtin(vm, "tigr.mouse", bu_tigr_mouse);
    register_builtin(vm, "tigr.touch", bu_tigr_touch);
    register_builtin(vm, "tigr.key_down", bu_tigr_key_down);
    register_builtin(vm, "tigr.key_held", bu_tigr_key_held);
    register_builtin(vm, "tigr.read_char", bu_tigr_read_char);
    register_builtin(vm, "tigr.show_keyboard", bu_tigr_show_keyboard);

    register_builtin(vm, "tigr.load_image", bu_tigr_load_image);
    register_builtin(vm, "tigr.load_image_mem", bu_tigr_load_image_mem);
    register_builtin(vm, "tigr.save_image", bu_tigr_save_image);

    register_builtin(vm, "tigr.time", bu_tigr_time);
    register_builtin(vm, "tigr.error", bu_tigr_error);
    register_builtin(vm, "tigr.read_file", bu_tigr_read_file);
    register_builtin(vm, "tigr.inflate", bu_tigr_inflate);
    register_builtin(vm, "tigr.decode_utf8", bu_tigr_decode_utf8);
    register_builtin(vm, "tigr.encode_utf8", bu_tigr_encode_utf8);

    // builtin font
    Int index = new_var(vm);
    data(index).pointer = tfont;
    hash_set(vm, "tigr.font", index);


    register_builtin(vm, "tigr.rgb", bu_tigr_rgb);
    register_builtin(vm, "tigr.rgba", bu_tigr_rgba);

}