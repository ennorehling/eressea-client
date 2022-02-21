#ifdef _MSC_VER
#define _CRT_SILENCE_NONCONFORMING_TGMATH_H
#endif

#include "viewport.h"

#include <tgmath.h>

/**
 * Divide one integer by another, rounding towards minus infinity.
 * @param x the dividend
 * @param y the divisor
 * @return the quoitant, rounded towards minus infinity
 */
int div_floor(int x, int y) {
    int q = x / y;
    int r = x % y;
    if ((r != 0) && ((r < 0) != (y < 0))) --q;
    return q;
}

int GetHexFromScreenY(int screen_x, int screen_y)
{
    (void)screen_x;
    screen_y += TILE_HEIGHT / 2;
    return - div_floor(screen_y, TILE_HEIGHT);
}

int GetHexFromScreenX(int screen_x, int screen_y)
{
    screen_x += TILE_WIDTH / 2;
    screen_y += TILE_HEIGHT / 2;
    screen_x += div_floor(screen_y, TILE_HEIGHT) * TILE_WIDTH / 2;
    return div_floor(screen_x, TILE_WIDTH);
}

int GetScreenFromHexY(int hex_x, int hex_y) {
    (void)hex_x;
    return -hex_y * TILE_HEIGHT;
}

int GetScreenFromHexX(int hex_x, int hex_y) {
    return (2 * hex_x + hex_y) * TILE_WIDTH / 2;
}

