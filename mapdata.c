#include "mapdata.h"
#include "stb/stb_ds.h"

size_t map_row_index(struct map_info** rows, int y)
{
    size_t i, len = arrlen(rows);
    // TODO: binary search
    for (i = 0; i != len; ++i) {
        if (rows[i]->y >= y) {
            return i;
        }
    }
    return len;
}

size_t map_col_index(struct map_info* row, int x)
{
    size_t i, len = arrlen(row);
    // TODO: binary search
    for (i = 0; i != len; ++i) {
        if (row[i].x >= x) {
            return i;
        }
    }
    return len;
}

map_info* map_get_row(map_info** rows, int y)
{
    size_t r, nrows = arrlen(rows);

    r = map_row_index(rows, y);
    if (r < nrows && rows[r]->y == y) {
        return rows[r];
    }
    return NULL;
}

map_info* map_get(map_info** rows, int x, int y)
{
    size_t r, nrows = arrlen(rows);

    r = map_row_index(rows, y);
    if (r < nrows && rows[r]->y == y) {
        map_info* row = rows[r];
        size_t c, ncols = arrlen(row);

        c = map_col_index(row, x);
        if (c < ncols && row[c].x == x) {
            return row + c;
        }
    }
    return NULL;
}

map_info* map_insert(struct map_info*** map, int x, int y)
{
    map_info** rows = *map;
    map_info* row = NULL;
    size_t p = 0, i = 0;

    if (rows == NULL) {

        arrins(rows, p, NULL);
        *map = rows;
        row = rows[p];
    }
    else {
        size_t len = arrlen(rows);
        size_t l = 0, r = len;
        while (l != r) {
            p = (l + r) / 2;
            map_info* match = rows[p];
            if (match->y == y) {
                break;
            }
            else if (match->y > y) {
                r = p;
            }
            else {
                l = p + 1;
            }
        }
        if (l == r) {
            p = l;
            arrins(rows, p, NULL);
            *map = rows;
        }
        row = rows[p];
    }

    if (row == NULL) {
        arrinsn(row, i, 1);
    }
    else {
        size_t len = arrlen(row);
        size_t l = 0, r = len;
        while (l != r) {
            i = (l + r) / 2;
            map_info* match = row + i;
            if (match->x == x) {
                return match;
            }
            else if (match->x > x) {
                r = i;
            }
            else {
                l = i + 1;
            }
        }
        if (l == r) {
            i = l;
            arrinsn(row, i, 1);
        }
    }
    rows[p] = row;
    row[i].x = x;
    row[i].y = y;
    return row + i;
}

void map_free(struct map_info** rows)
{
    size_t i, len = arrlen(rows);
    for (i = 0; i != len; ++i) {
        arrfree(rows[i]);
    }
    arrfree(rows);
}
