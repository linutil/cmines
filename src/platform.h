#pragma once

typedef void* texture_t;
typedef void* audio_t;

void platform_init();
void platform_close();

int render_should_close();

void platform_begin_frame();
void platform_end_frame();

texture_t platform_load_texture(const char* path);
audio_t platform_load_audio(const char* path);

void platform_draw_texture(texture_t texture, int x, int y);

float platform_cursor_x();
float platform_cursor_y();

int platform_mb_right();
int platform_mb_left();