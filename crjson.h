#pragma once

struct cJSON;

struct cJSON *crjson_load(const char *filename);
struct cJSON *crjson_get_faction(struct cJSON *cr, int no);
struct cJSON *crjson_get_region(struct cJSON *cr, int no);
struct cJSON *crjson_get_region_at(struct cJSON *cr, int x, int y, int z);

struct cJSON *crjson_get_unit(struct cJSON *cr, int no, struct cJSON *region);
struct cJSON *crjson_get_ship(struct cJSON *cr, int no, struct cJSON *region);
struct cJSON *crjson_get_building(struct cJSON *cr, int no, struct cJSON *region);

char *int_to_id(int no);
