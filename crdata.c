#pragma once

#include "crdata.h"
#include "crjson.h"

#include <cJSON.h>
#include <stdio.h>

struct crdata {
    cJSON *json;
};

crdata *cr_load(const char *filename) {
    cJSON *json = crjson_load(filename);
    if (json) {
        crdata *cr = malloc(sizeof(crdata));
        cr->json = json;
        return cr;
    }
    return NULL;
}

cJSON *cr_get_region_at(crdata *cr, int x, int y, int z) {
    return crjson_get_region_at(cr->json, x, y, z);
}
