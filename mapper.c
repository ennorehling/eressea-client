#include <raylib.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Load an image from "fname" and return an SDL_Texture with the content of the image
Texture2D load_texture(const char* fname);

#define MAXTERRAINS 4
static Texture2D textures[MAXTERRAINS];

void load_textures(void) {
    textures[0] = load_texture("res/img/ozean.png");
    textures[1] = load_texture("res/img/ebene.png");
    textures[2] = load_texture("res/img/berge.png");
    textures[3] = load_texture("res/img/wueste.png");
}

void destroy_textures(void) {
    int i;
    for (i = 0; i != MAXTERRAINS; ++i) {
        UnloadTexture(textures[i]);
    }
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
