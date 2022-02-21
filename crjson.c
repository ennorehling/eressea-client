#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "log.h"

#include "crjson.h"
#include "crfile.h"

#include "cjson/cJSON.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define CRHASHSIZE 31

static int cr_turn;

char *itoab_r(int value, char *buffer, int radix, size_t len)
{
    char *dst;

    assert(len > 2);
    dst = buffer + len - 2;
    dst[1] = 0;
    if (value != 0) {
        int neg = 0;

        if (value < 0) {
            value = -value;
            neg = 1;
        }
        while (value && dst >= buffer) {
            int x = value % radix;
            value = value / radix;
            if (x < 10) {
                *(dst--) = (char)('0' + x);
            }
            else if ('a' + x - 10 == 'l') {
                *(dst--) = 'L';
            }
            else {
                *(dst--) = (char)('a' + (x - 10));
            }
        }
        if (dst > buffer) {
            if (neg) {
                *(dst) = '-';
            }
            else {
                ++dst;
            }
        }
        else {
            log_error(NULL, "static buffer exhausted, itoab(%d, %d)", value, radix);
            assert(value == 0 || !"itoab: static buffer exhausted");
        }
    }
    else {
        dst[0] = '0';
    }

    return dst;
}

char *int_to_id(int no) {
    static char sbuf[12];
    char *s;
    int i;
#ifdef _MSC_VER
    s = _itoa(no, sbuf, 36);
#else
    s = itoab_r(no, sbuf, 36, sizeof(sbuf));
#endif
    for (i = 0; s[i]; ++i) {
        if (s[i] == 'l') s[i] = 'L';
    }
    return s;
}

struct cJSON *crjson_load(const char *filename) {
    FILE * F;
    cJSON *data;

    F = fopen(filename, "rt");
    if (!F) {
        return NULL;
    }
    data = crfile_read(F, filename);
    fclose(F);
    return data;
}

static cJSON *crjson_get_child_id(cJSON *arr, int no) {
    cJSON *child;
    if (arr != NULL) {
        assert(arr->type == cJSON_Array);
        for (child = arr->child; child; child = child->next) {
            cJSON *item;
            assert(child->type == cJSON_Object);
            item = cJSON_GetObjectItem(child, "id");
            if (item && item->valueint == no) {
                return child;
            }
        }
    }
    return NULL;
}

struct cJSON * crjson_get_faction(struct cJSON *cr, int no)
{
    cJSON *arr = cJSON_GetObjectItem(cr, "PARTEI");

    return crjson_get_child_id(arr, no);
}

struct cJSON * crjson_get_region(struct cJSON *cr, int no) {
    cJSON *arr = cJSON_GetObjectItem(cr, "REGION");
    return crjson_get_child_id(arr, no);
}

struct cJSON * crjson_get_region_at(struct cJSON *cr, int x, int y, int z) {
    cJSON *child, *arr = cJSON_GetObjectItem(cr, "REGION");

    assert(arr->type == cJSON_Array);
    for (child = arr->child; child; child = child->next) {
        cJSON *item;
        assert(child->type == cJSON_Object);
        if (z != 0) {
            item = cJSON_GetObjectItem(child, "z");
            if (item == NULL || item->valueint != z) {
                continue;
            }
        }
        item = cJSON_GetObjectItem(child, "x");
        if (item == NULL || item->valueint != x) {
            continue;
        }
        item = cJSON_GetObjectItem(child, "y");
        if (item == NULL || item->valueint != y) {
            continue;
        }
        return child;
    }
    return NULL;
}

static struct cJSON * crjson_get_region_child(struct cJSON *cr, const char *type, int no, struct cJSON *region) {
    cJSON *arr, *child;
    if (region) {
        assert(region->type == cJSON_Object);
        arr = cJSON_GetObjectItem(region, type);
        return crjson_get_child_id(arr, no);
    }
    arr = cJSON_GetObjectItem(region, "REGION");
    for (child = arr->child; child; child = child->next) {
        cJSON *unit = crjson_get_region_child(cr, type, no, child);
        if (unit) {
            return unit;
        }
    }
    return NULL;
}

struct cJSON * crjson_get_unit(struct cJSON *cr, int no, struct cJSON *region)
{
    return crjson_get_region_child(cr, "EINHEIT", no, region);
}

struct cJSON * crjson_get_ship(struct cJSON *cr, int no, struct cJSON *region) {
    return crjson_get_region_child(cr, "SCHIFF", no, region);
}

struct cJSON * crjson_get_building(struct cJSON *cr, int no, struct cJSON *region) {
    return crjson_get_region_child(cr, "BURG", no, region);
}
