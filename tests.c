#if !defined(_CRT_SECURE_NO_WARNINGS) && defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "mapdata.h"
#include "crfile.h"
#include "viewport.h"

#include "stb/stb_ds.h"
#include "CuTest.h"

#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>

static void test_map_insert(CuTest* tc)
{
    map_info* ins;
    map_data map = { NULL };

    CuAssertPtrNotNull(tc, (ins = map_insert(&map.rows, 1, 0)));
    CuAssertPtrNotNull(tc, map.rows);
    CuAssertPtrEquals(tc, ins, map.rows[0]);
    CuAssertIntEquals(tc, 1, (int)arrlen(map.rows[0]));
    CuAssertPtrNotNull(tc, map.rows);
    CuAssertIntEquals(tc, 0, map.rows[0][0].y);
    CuAssertIntEquals(tc, 1, map.rows[0][0].x);

    CuAssertPtrNotNull(tc, (ins = map_insert(&map.rows, 0, 0)));
    CuAssertIntEquals(tc, 2, (int)arrlen(map.rows[0]));
    CuAssertIntEquals(tc, 0, map.rows[0][0].y);
    CuAssertIntEquals(tc, 0, map.rows[0][0].x);
    CuAssertIntEquals(tc, 0, map.rows[0][1].y);
    CuAssertIntEquals(tc, 1, map.rows[0][1].x);

    map_free(map.rows);
}

static void test_map_get_row(CuTest* tc)
{
    map_data map = { NULL };
    map_info* row;
    CuAssertPtrEquals(tc, NULL, map_get_row(map.rows, 0));
    map_insert(&map.rows, 0, 0);
    CuAssertPtrNotNull(tc, (row = map_get_row(map.rows, 0)));
    CuAssertIntEquals(tc, 0, row->y);
    CuAssertIntEquals(tc, 1, (int)arrlen(row));

    map_free(map.rows);
}

static void test_map_get(CuTest* tc)
{
    map_info* ins;
    map_data map = { NULL };

    CuAssertPtrEquals(tc, NULL, map_get(map.rows, 0, 0));

    map_insert(&map.rows, 0, 0);
    CuAssertPtrNotNull(tc, (ins = map_get(map.rows, 0, 0)));
    CuAssertIntEquals(tc, 0, ins->x);
    CuAssertIntEquals(tc, 0, ins->y);

    map_insert(&map.rows, 1, 0);
    CuAssertPtrNotNull(tc, (ins = map_get(map.rows, 1, 0)));
    CuAssertIntEquals(tc, 1, ins->x);
    CuAssertIntEquals(tc, 0, ins->y);

    map_free(map.rows);
}

static void test_map_row_index(CuTest* tc)
{
    map_data map = { NULL };

    CuAssertIntEquals(tc, 0, (int)map_row_index(map.rows, 0));

    map_insert(&map.rows, 1, 1);
    CuAssertIntEquals(tc, 0, (int)map_row_index(map.rows, 0));
    CuAssertIntEquals(tc, 0, (int)map_row_index(map.rows, 1));
    CuAssertIntEquals(tc, 1, (int)map_row_index(map.rows, 2));

    map_insert(&map.rows, 1, 0);
    CuAssertIntEquals(tc, 0, (int)map_row_index(map.rows, 0));
    CuAssertIntEquals(tc, 1, (int)map_row_index(map.rows, 1));
    CuAssertIntEquals(tc, 2, (int)map_row_index(map.rows, 2));

    map_free(map.rows);
}

static void test_map_col_index(CuTest* tc)
{
    map_data map = { NULL };
    map_info* row = NULL;

    CuAssertIntEquals(tc, 0, (int)map_col_index(row, 0));

    map_insert(&map.rows, 1, 0);
    row = map.rows[0];
    CuAssertIntEquals(tc, 0, (int)map_col_index(row, 0));
    CuAssertIntEquals(tc, 0, (int)map_col_index(row, 1));
    CuAssertIntEquals(tc, 1, (int)map_col_index(row, 2));

    map_insert(&map.rows, 0, 0);
    CuAssertIntEquals(tc, 0, (int)map_col_index(row, 0));
    CuAssertIntEquals(tc, 1, (int)map_col_index(row, 1));
    CuAssertIntEquals(tc, 2, (int)map_col_index(row, 2));

    map_free(map.rows);
}

static void test_crparse(CuTest* tc) {
    FILE* F;
    const char* filename = "tests/effects.cr";

    F = fopen(filename, "r");
    if (F) {
        struct cJSON* json, *child, *region;
        json = crfile_read(F, filename);
        CuAssertPtrNotNull(tc, json);
        CuAssertIntEquals(tc, cJSON_Object, json->type);

        child = cJSON_GetObjectItem(json, "PARTEI");
        CuAssertIntEquals(tc, cJSON_Array, child->type);
        CuAssertIntEquals(tc, 1, cJSON_GetArraySize(child));

        region = cJSON_GetObjectItem(json, "REGION");
        CuAssertIntEquals(tc, cJSON_Array, region->type);
        CuAssertIntEquals(tc, 2, cJSON_GetArraySize(region));

        region = cJSON_GetArrayItem(region, 0);
        CuAssertIntEquals(tc, cJSON_Object, region->type);
        
        child = cJSON_GetObjectItem(region, "EFFECTS");
        CuAssertIntEquals(tc, cJSON_Array, child->type);
        CuAssertIntEquals(tc, 2, cJSON_GetArraySize(child));

        child = cJSON_GetObjectItem(region, "BURG"); // Liste der Burgen
        CuAssertIntEquals(tc, cJSON_Array, child->type);
        CuAssertIntEquals(tc, 1, cJSON_GetArraySize(child));

        child = cJSON_GetArrayItem(child, 0); // Eine Burg
        CuAssertIntEquals(tc, cJSON_Object, child->type);

        child = cJSON_GetObjectItem(child, "EFFECTS"); // Effekte der Burg
        CuAssertIntEquals(tc, cJSON_Array, child->type);
        CuAssertIntEquals(tc, 1, cJSON_GetArraySize(child));

        cJSON_Delete(json);
        fclose(F);
    }
}

static void test_get_screen_from_hex(CuTest* tc) {
    CuAssertIntEquals(tc, 0, GetScreenFromHexX(0, 0));
    CuAssertIntEquals(tc, - TILE_WIDTH / 2, GetScreenFromHexX(0, -1));
    CuAssertIntEquals(tc, TILE_WIDTH / 2, GetScreenFromHexX(0, 1));
    CuAssertIntEquals(tc, TILE_WIDTH, GetScreenFromHexX(1, 0));
    CuAssertIntEquals(tc, -TILE_WIDTH, GetScreenFromHexX(-1, 0));

    CuAssertIntEquals(tc, 0, GetScreenFromHexY(0, 0));
    CuAssertIntEquals(tc, -TILE_HEIGHT, GetScreenFromHexY(0, 1));
    CuAssertIntEquals(tc, TILE_HEIGHT, GetScreenFromHexY(0, -1));
}

static void test_get_hex_from_screen(CuTest* tc) {
    CuAssertIntEquals(tc, 0, GetHexFromScreenX(0, 0));
    CuAssertIntEquals(tc, 1, GetHexFromScreenX(TILE_WIDTH, 0));
    CuAssertIntEquals(tc, -1, GetHexFromScreenX(-TILE_WIDTH, 0));

    CuAssertIntEquals(tc, 0, GetHexFromScreenX(TILE_WIDTH / 2 - 1, 0));
    CuAssertIntEquals(tc, 1, GetHexFromScreenX(TILE_WIDTH / 2, 0));

    CuAssertIntEquals(tc, 0, GetHexFromScreenX(- TILE_WIDTH / 2, 0));
    CuAssertIntEquals(tc, -1, GetHexFromScreenX(- TILE_WIDTH / 2 - 1, 0));

    CuAssertIntEquals(tc, 1, GetHexFromScreenX(0, TILE_HEIGHT));

    CuAssertIntEquals(tc, 0, GetHexFromScreenY(0, 0));
    CuAssertIntEquals(tc, -1, GetHexFromScreenY(0, TILE_HEIGHT / 2));
    CuAssertIntEquals(tc, 0, GetHexFromScreenY(0, TILE_HEIGHT / 2 - 1));

    CuAssertIntEquals(tc, 0, GetHexFromScreenY(0, - TILE_HEIGHT / 2));
    CuAssertIntEquals(tc, 1, GetHexFromScreenY(0, - TILE_HEIGHT / 2 - 1));
    
    CuAssertIntEquals(tc, -6, GetHexFromScreenY(-400, 300));
    CuAssertIntEquals(tc, -3, GetHexFromScreenX(-400, 300));
}

void add_suite_mapdata(CuSuite* suite)
{
    SUITE_ADD_TEST(suite, test_map_insert);
    SUITE_ADD_TEST(suite, test_map_row_index);
    SUITE_ADD_TEST(suite, test_map_col_index);
    SUITE_ADD_TEST(suite, test_map_get_row);
    SUITE_ADD_TEST(suite, test_map_get);
    SUITE_ADD_TEST(suite, test_get_screen_from_hex);
    SUITE_ADD_TEST(suite, test_get_hex_from_screen);
    SUITE_ADD_TEST(suite, test_crparse);
}

int main(void)
{
    CuString* output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    add_suite_mapdata(suite);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}

