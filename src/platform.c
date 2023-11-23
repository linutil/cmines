#include <raylib.h>
#include <stdio.h>
#include "platform.h"
#include "build_settings.h"
#include <stdlib.h>

// All audio functions should check has_audio.
// This is to ensure that we still run on backends without support.
int has_audio;

Texture2D textures[1024];
Sound audio[1024];

Texture2D* texture_accessor = textures;
Sound* audio_accessor = audio;

void platform_init() {

#if !RL_LOG
    SetTraceLogLevel(LOG_NONE);
#endif

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "cmines");

    if(!IsWindowReady()) {
        fprintf(stderr, "Could not initialize platform window\n");
        exit(EXIT_FAILURE);
    }

    InitAudioDevice();

#if PLATFORM_AUDIO
    has_audio = IsAudioDeviceReady();
#else
    has_audio = 0;
#endif

    if(!has_audio) {
        fprintf(stderr, "Could not initialize platform audio\n");
    }
}

void platform_close() {

    printf("Cleaning up resources");

    size_t texture_count = (size_t)texture_accessor - (size_t)textures;
    for (size_t i = 0; i < texture_count; ++i) {
        UnloadTexture(textures[i]);
    }

    if(has_audio) {
        size_t sound_count = (size_t)audio_accessor - (size_t)audio;
        for (size_t i = 0; i < sound_count; ++i) {
            UnloadSound(audio[i]);
        }
    }

    CloseWindow();
    if(has_audio)
        CloseAudioDevice();
}

int render_should_close() {
    return WindowShouldClose();
}

void platform_begin_frame() {
    BeginDrawing();
    ClearBackground(BLACK);
}

void platform_end_frame() {
    EndDrawing();
}

texture_t platform_load_texture(const char* path) {
    Texture2D texture = LoadTexture(path);
    *texture_accessor = texture;
    texture_t ret = texture_accessor;
    texture_accessor++;
    return ret;
}

audio_t platform_load_audio(const char* path) {
    if(!has_audio)
        return NULL;
    Sound sound = LoadSound(path);
    *audio_accessor = sound;
    audio_t ret = audio_accessor;
    audio_accessor++;
    return ret;
}

void platform_draw_texture(texture_t texture, int x, int y) {
    DrawTexture(*((Texture2D*)texture), x, y, WHITE);
}

float platform_cursor_x() {
    return (float)GetMouseX();
}

float platform_cursor_y() {
    return (float)GetMouseY();
}

int platform_mb_right() {
    return IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
}

int platform_mb_left() {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}