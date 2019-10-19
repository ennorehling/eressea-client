#include <SDL.h>
#include <SDL_image.h>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "imgui_sdl/imgui_sdl.h"

#include <stdbool.h>
#include <stdio.h>

// Manage error messages
void check_error_sdl(bool check, const char* message);
void check_error_sdl_img(bool check, const char* message);

// Load an image from "fname" and return an SDL_Texture with the content of the image
SDL_Texture* load_texture(const char* fname, SDL_Renderer *renderer);

#define MAXTERRAINS 4
static SDL_Texture* textures[MAXTERRAINS];

void load_textures(SDL_Renderer *renderer) {
    textures[0] = load_texture("res/img/ozean.png", renderer);
    textures[1] = load_texture("res/img/ebene.png", renderer);
    textures[2] = load_texture("res/img/berge.png", renderer);
    textures[3] = load_texture("res/img/wueste.png", renderer);
}

void destroy_textures(void) {
    int i;
    for (i = 0; i != MAXTERRAINS; ++i) {
        SDL_DestroyTexture(textures[i]);
    }
}

static int map_terrain(int x, int y) {
    int tx = abs(x);
    int ty = abs(y);
    return ((tx < ty) ? tx : ty) % 4;
}

int main(int argc, char** argv) {
    // Initialize SDL
    check_error_sdl(SDL_Init(SDL_INIT_VIDEO) != 0, "Unable to initialize SDL");

    // Create and initialize a 800x600 window
    SDL_Window* window = SDL_CreateWindow("Test SDL 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    check_error_sdl(window == NULL, "Unable to create window");

    // Create and initialize a hardware accelerated renderer that will be refreshed in sync with your monitor (at approx. 60 Hz)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    check_error_sdl(renderer == NULL, "Unable to create a renderer");

    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, 800, 600);
    ImGui_ImplSDL2_InitForD3D(window);

    // Set the default renderer color to corn blue
    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);

    // Initialize SDL_img
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    check_error_sdl_img((initted & flags) != flags, "Unable to initialize SDL_image");

    load_textures(renderer);
    SDL_Event e;
    SDL_Rect viewport = { 0 };
    SDL_Point delta = { 0 };
    int dw, dh;
    SDL_GetWindowSize(window, &viewport.w, &viewport.h);
    dw = 2 + viewport.w / 64;
    dh = 2 + viewport.h / 48;
    bool quit = false;
    int speed = 1;
    Uint32 slowest = 0, fastest = 1000;
    while (!quit) {
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

        Uint32 start = SDL_GetTicks();

        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::Begin("Xontormia (4,7)");
        ImGui::Text("Ebene");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        ImGui::Columns(2);
        ImGui::Separator();
        ImGui::Text("Baeume:");
        ImGui::NextColumn();
        ImGui::Text("12");
        ImGui::NextColumn();
        ImGui::Text("Bauern:");
        ImGui::NextColumn();
        ImGui::Text("3890");
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
        ImGui::End();

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
        Uint32 stop = SDL_GetTicks();
        if (stop - start > slowest) {
            slowest = stop - start;
            printf("new slowest frame: %u msec\n", slowest);
        }
        if (stop - start < fastest) {
            fastest = stop - start;
            printf("new fastest frame: %u msec\n", fastest);
        }
        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
        // Update the window surface (show the renderer)
        SDL_RenderPresent(renderer);
    }
    // Wait for 10 seconds
    // SDL_Delay(10000);

    // Clear the allocated resources
    destroy_textures();

    ImGui_ImplSDL2_Shutdown();
    ImGuiSDL::Deinitialize();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

// In case of error, print the error code and close the application
void check_error_sdl(bool check, const char* message) {
    if (check) {
        fputs(message, stderr);
        fputc(' ', stderr);
        fputs(SDL_GetError(), stderr);
        fputc('\n', stderr);
        SDL_Quit();
        exit(-1);
    }
}

// In case of error, print the error code and close the application
void check_error_sdl_img(bool check, const char* message) {
    if (check) {
        fputs(message, stderr);
        fputc(' ', stderr);
        fputs(IMG_GetError(), stderr);
        fputc('\n', stderr);
        IMG_Quit();
        SDL_Quit();
        exit(-1);
    }
}

// Load an image from "fname" and return an SDL_Texture with the content of the image
SDL_Texture* load_texture(const char* fname, SDL_Renderer *renderer) {
    SDL_Surface *image = IMG_Load(fname);
    check_error_sdl_img(image == NULL, "Unable to load image");
    SDL_Texture *img_texture = SDL_CreateTextureFromSurface(renderer, image);
    check_error_sdl(img_texture == NULL, "Unable to create a texture from the image");
    SDL_FreeSurface(image);
    return img_texture;
}
