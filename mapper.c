#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <raylib.h>

#include "stb/tests/prerelease/stb_lib.h"
#include "stb/stb_ds.h"

#include "crfile.h"
#include "mapdata.h"
#include "viewport.h"
#include "sds/sds.h"

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

typedef struct Terrain {
    Texture2D texture;
    sds filename;
    sds* names;
} Terrain;

// Load an image from "fname" and return an SDL_Texture with the content of the image
Texture2D load_texture(const char* fname);

static Terrain *terrains;

void load_textures(void) {
    char **p;
    char ** files = stb_readdir_files_mask("res/img", "*.png");
    size_t len = stb_arr_len(files);
    stb_arr_for(p, files) {
        char *filename = *p;
        char* lp = filename + 8;
        char* rp = strchr(lp, '.');
        if (rp > lp) {
            Texture2D tex = load_texture(filename);
            if (tex.format) {
                Terrain t;
                t.filename = sdsnewlen(lp, rp - lp);
                t.texture = tex;
                t.names = NULL;
                stb_arr_push(terrains, t);
            }
        }
    }
    stb_readdir_free(files);
}

void load_terrains(const char * filename) {
    FILE* F = fopen(filename, "rt");
    if (F) {
        while (!feof(F)) {
            char buffer[128];
            char* token;
            if (NULL == fgets(buffer, sizeof buffer, F)) {
                break;
            }
            token = strtok(buffer, ",;");
            if (token) {
                size_t len = arrlen(terrains);
                for (unsigned int i = 0; i != len; ++i) {
                    if (strcmp(terrains[i].filename, token) == 0) {
                        while ((token = strtok(NULL, ",;")) != NULL) {
                            char* name = sdsnew(token);
                            arrpush(terrains[i].names, name);
                        }
                        break;
                    }
                }
            }
        }
        fclose(F);
    }
}

void destroy_textures(void) {
    for (unsigned int i = 0; i != stb_arr_len(terrains); ++i)
    {
        UnloadTexture(terrains[i].texture);
        sdsfree(terrains[i].filename);
        for (unsigned int j = 0; j != stb_arr_len(terrains[i].names); ++j)
        {
            sdsfree(terrains[i].names[j]);
        }
        arrfree(terrains[i].names);
    }
    stb_arr_free(terrains);
    terrains = NULL;
}

static int terrain_index(const char* name)
{
    unsigned int num_terrains = stb_arr_len(terrains);
    if (num_terrains == 0) {
        return 0;
    }
    if (strcmp(name, "Ozean") == 0) {
        return num_terrains;
    }
    return 1 + name[0] % (num_terrains - 1);
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
                    ins->terrain_index = terrain_index(attr->valuestring);
                    // ins->terrain_index = 1 + abs(y) % stb_arr_len(terrains);
                }
            }
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
    // Create and initialize a 800x600 window
    InitWindow(800, 600, "raylib mapper");
    load_textures();
    load_terrains("res/terrains.txt");
    if (argc > 0) {
        const char* filename = argv[1];
        load_map(filename);
    }
    Rect viewport = { 0, 0, 0, 0 };
    viewport.width = GetScreenWidth();
    viewport.height = GetScreenHeight();
    int map_width = 2 + viewport.width / TILE_WIDTH;
    int map_height = 2 + viewport.height / TILE_HEIGHT;
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        int speed = 10;
        Point delta = { 0, 0 };
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            speed = 50;
        }
        else if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
            speed = 1;
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

        /* screen origin is at bottom left */
        int origin_x = viewport.x - viewport.width / 2;
        int origin_y = viewport.y + viewport.height / 2;
        int map_left = GetHexFromScreenX(origin_x, origin_y);
        int map_bottom = GetHexFromScreenY(origin_x, origin_y);

        BeginDrawing();
        ClearBackground(WHITE);

        size_t nrows = arrlen(map.rows);
        for (unsigned int r = map_row_index(map.rows, map_bottom); r < nrows; ++r) {
            map_info* row = map.rows[r];
            if (row->y > map_bottom + map_height) {
                // this row is the first one above the screen
                break;
            }
            int map_x = map_left - (1 + row->y - map_bottom) / 2;

            Vector2 dest;
            dest.y = (float)GetScreenFromHexY(map_x, row->y) - viewport.y - IMAGE_HEIGHT / 2 + viewport.height / 2;

            size_t ncols = arrlen(row);
            for (unsigned int c = map_col_index(row, map_x); c < ncols; ++c) {
                map_info* tile = row + c;
                if (tile->x > map_x + map_width) {
                    // first tile to the right of the screen
                    break;
                }
                int terrain = tile->terrain_index;
                if (terrain > 0) {
                    dest.x = (float)GetScreenFromHexX(tile->x, tile->y) - viewport.x - IMAGE_WIDTH / 2 + viewport.width / 2;;
                    DrawTextureV(terrains[terrain - 1].texture, dest, WHITE);
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
