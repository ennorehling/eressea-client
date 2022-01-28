#include <raylib.h>

#define STB_LIB_IMPLEMENTATION
#include "stb/tests/prerelease/stb_lib.h"
#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Load an image from "fname" and return an SDL_Texture with the content of the image
Texture2D load_texture(const char* fname);

static Texture2D *textures;

void load_textures(void) {
    char **files, **p;
    files = stb_readdir_files_mask("res/img", "*.png");
    stb_arr_for(p, files) {
        char *filename = *p;
        Texture2D tex = load_texture(filename);
        if (tex.format) {
            stb_arr_push(textures, tex);
        }
    }
    stb_readdir_free(files);
}

void destroy_textures(void) {
    int i;
    for (i = 0; i != stb_arr_len(textures); ++i) {
        UnloadTexture(textures[i]);
    }
    stb_arr_free(textures);
}

static int map_terrain(int x, int y) {
    int tx = abs(x);
    int ty = abs(y);
    return ((tx < ty) ? tx : ty) % 4;
}

int main(int argc, char** argv) {
    // Create and initialize a 800x600 window
    InitWindow(800, 600, "raylib mapper");
    load_textures();
    Rectangle viewport = { 0, 0, 0, 0 };
    int dw, dh;
    viewport.width = GetScreenWidth();
    viewport.height = GetScreenHeight();
    dw = 2 + viewport.width / 64;
    dh = 2 + viewport.height / 48;
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        int speed = 1;
        Vector2 delta = { 0, 0 };
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            speed = 10;
        }
        if (IsKeyDown(KEY_LEFT)) {
            delta.x = -1;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            delta.x = 1;
        }
        if (IsKeyDown(KEY_UP)) {
            delta.y = -1;
        }
        if (IsKeyDown(KEY_DOWN)) {
            delta.y = 1;
        }
        viewport.x += delta.x * speed;
        viewport.y += delta.y * speed;

        float dx, dy;
        dx = viewport.x / 64 - ((viewport.x < 0) ? 1 : 0);
        dy = viewport.y / 48 - ((viewport.y < 0) ? 1 : 0);

        BeginDrawing();
        ClearBackground(WHITE);
        int x, y;
        for (y = 0; y <= dh; ++y) {
            Vector2 dest;
            int ty = dy + y;
            dest.y = ty * 48 - viewport.y - 40;
            for (x = 0; x <= dw; ++x) {
                int tx = dx + x;
                dest.x = tx * 64 - viewport.x - 40 + (ty & 1) * 32;
                // Copy the texture on the renderer
                int terrain = map_terrain(tx, ty);
                DrawTextureV(textures[terrain], dest, WHITE);
            }
        }
        EndDrawing();
    }

    // Clear the allocated resources
    destroy_textures();
    CloseWindow();

    return 0;
}

// Load an image from "fname" and return an SDL_Texture with the content of the image
Texture2D load_texture(const char* fname) {
    return LoadTexture(fname);
}
