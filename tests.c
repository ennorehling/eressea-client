#include "mapdata.h"

#include "stb/stb_ds.h"
#include "CuTest.h"

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

void add_suite_mapdata(CuSuite* suite)
{
    SUITE_ADD_TEST(suite, test_map_insert);
    SUITE_ADD_TEST(suite, test_map_row_index);
    SUITE_ADD_TEST(suite, test_map_col_index);
    SUITE_ADD_TEST(suite, test_map_get_row);
    SUITE_ADD_TEST(suite, test_map_get);
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

