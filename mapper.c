#include <raylib.h>

#include <stdbool.h>
#include <stdio.h>

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
    Vector2 delta = { 0, 0 };
    int dw, dh;
    viewport.width = GetScreenWidth();
    viewport.height = GetScreenHeight();
    dw = 2 + viewport.width / 64;
    dh = 2 + viewport.height / 48;
    int speed = 1;
    while (!WindowShouldClose()) {
        while (SDL_PollEvent(&e) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_LSHIFT:
                case SDL_SCANCODE_RSHIFT:
                    speed = 1;
                    break;
                case SDL_SCANCODE_LEFT:
                case SDL_SCANCODE_RIGHT:
                    delta.x = 0;
                    break;
                case SDL_SCANCODE_UP:
                case SDL_SCANCODE_DOWN:
                    delta.y = 0;
                    break;
                default:
                    break;
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_LSHIFT:
                case SDL_SCANCODE_RSHIFT:
                    speed = 10;
                    break;
                case SDL_SCANCODE_ESCAPE:
                    // quit = true;
                    break;
                case SDL_SCANCODE_LEFT:
                    delta.x = -1;
                    break;
                case SDL_SCANCODE_RIGHT:
                    delta.x = 1;
                    break;
                case SDL_SCANCODE_UP:
                    delta.y = -1;
                    break;
                case SDL_SCANCODE_DOWN:
                    delta.y = 1;
                    break;
		        default:
		            break;
                }
            }
        }
        viewport.x += delta.x * speed;
        viewport.y += delta.y * speed;

        // Clear the window content (using the default renderer color)
        SDL_RenderClear(renderer);

        // We need to create a destination rectangle for the image (where we want this to be show) on the renderer area
        SDL_Rect dest_rect;
        dest_rect.w = 80; dest_rect.h = 80;

        int dx, dy;
        dx = viewport.x / 64 - ((viewport.x < 0) ? 1 : 0);
        dy = viewport.y / 48 - ((viewport.y < 0) ? 1 : 0);

        int x, y;
        for (y = 0; y != dh; ++y) {
            int ty = dy + y;
            dest_rect.y = ty * 48 - viewport.y - 40;
            for (x = 0; x != dw; ++x) {
                int tx = dx + x;
                dest_rect.x = tx * 64 - viewport.x - 40 + (ty & 1) * 32;
                // Copy the texture on the renderer
                int terrain = map_terrain(tx, ty);
                SDL_RenderCopy(renderer, textures[terrain], NULL, &dest_rect);
            }
        }
    }
    // Wait for 10 seconds
    // SDL_Delay(10000);

    // Clear the allocated resources
    destroy_textures();
    CloseWindow();

    return 0;
}

// Load an image from "fname" and return an SDL_Texture with the content of the image
Texture2D load_texture(const char* fname) {
    return LoadTexture(fname);
}
