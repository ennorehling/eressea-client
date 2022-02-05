#pragma once

struct cJSON;

typedef struct map_info {
    unsigned int report_index;
    int x, y;
    unsigned int terrain_index;
    const char* name;
} map_info;

struct map_info* map_get(struct map_info** rows, int x, int y);
struct map_info* map_get_row(struct map_info** rows, int y);
size_t map_row_index(struct map_info** rows, int y);
size_t map_col_index(struct map_info* row, int x);
void map_free(struct map_info** rows);

struct map_info* map_insert(struct map_info*** map, int x, int y);

typedef struct map_data {
    map_info** rows;
    struct cJSON* report;
    struct cJSON** regions;
} map_data;

