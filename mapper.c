#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <raylib.h>

#include "stb/tests/prerelease/stb_lib.h"
#include "stb/stb_ds.h"

#include "crfile.h"
#include "mapdata.h"

#include <cJSON/cJSON.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Point {
    int x, y;
} Point;

typedef struct Rect {
    int x, y, width, height;
} Rect;

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

static int report_x(int x, int y) {
    return x + (y + 1) / 2;
}

static int map_terrain(int x, int y) {
    int tx = report_x(x, y);
    int ty = -y;
    return tx % stb_arr_len(textures);
}

static map_data map;

int load_map(const char* filename) {
    FILE* F = fopen(filename, "r");
    if (F) {
        struct cJSON *report;
        report = crfile_read(F, filename);
        fclose(F);
        if (report && report->type == cJSON_Object) {
            cJSON* jRegions = cJSON_GetObjectItem(report, "REGION");
            map.report = report;
            map.regions = NULL;
            map.rows = NULL;
            if (jRegions && jRegions->type == cJSON_Array) {
                cJSON* jRegion;
                unsigned int i = 0;
                int num = cJSON_GetArraySize(jRegions);

                map.regions = arraddnptr(map.regions, num);
                cJSON_ArrayForEach(jRegion, jRegions) {
                    int x, y;
                    struct map_info* ins;
                    struct cJSON* attr;
                    x = cJSON_GetObjectItem(jRegion, "x")->valueint;
                    y = cJSON_GetObjectItem(jRegion, "y")->valueint;
                    map.regions[i++] = jRegion;
                    ins = map_insert(&map.rows, x, y);
                    attr = cJSON_GetObjectItem(jRegion, "Name");
                    ins->name = attr ? attr->valuestring : NULL;
                    attr = cJSON_GetObjectItem(jRegion, "Terrain");
                    ins->terrain_index = 0;
                }
            }
        }
    }
    return 0;
}

int main(int argc, char** argv) {

    if (argc > 0) {
        load_map(argv[1]);
        const char* filename = argv[1];
        load_map(filename);
    }
    // Create and initialize a 800x600 window
    InitWindow(800, 600, "raylib mapper");
    load_textures();
    Rect viewport = { 0, 0, 0, 0 };
    int dw, dh;
    viewport.width = GetScreenWidth();
    viewport.height = GetScreenHeight();
    dw = 2 + viewport.width / 64;
    dh = 2 + viewport.height / 48;
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        int speed = 1;
        Point delta = { 0, 0 };
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

        int dx, dy;
        dx = viewport.x / 64 - ((viewport.x < 0) ? 1 : 0);
        dy = viewport.y / 48 - ((viewport.y < 0) ? 1 : 0);

        BeginDrawing();
        ClearBackground(WHITE);
        int x, y;
        for (y = 0; y <= dh; ++y) {
            Vector2 dest;
            int ty = dy + y;
            dest.y = (float)(ty * 48 - viewport.y - 40);
            for (x = 0; x <= dw; ++x) {
                int tx = dx + x;
                dest.x = (float)(tx * 64 - viewport.x - 40 + (ty & 1) * 32);
                // Copy the texture on the renderer
                if (tx || ty) {
                    int terrain = map_terrain(tx, ty);
                    DrawTextureV(textures[terrain], dest, WHITE);
                }
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
