#pragma once

struct cJSON;
typedef struct crdata crdata;

struct crdata *cr_load(const char *filename);
struct cJSON *cr_get_region_at(crdata *cr, int x, int y, int z);
