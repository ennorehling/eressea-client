#pragma once

#define IMAGE_WIDTH 80
#define IMAGE_HEIGHT 80
#define TILE_WIDTH 64
#define TILE_HEIGHT 48

int GetScreenFromHexY(int hex_x, int hex_y);
int GetScreenFromHexX(int hex_x, int hex_y);
int GetHexFromScreenX(int screen_x, int screen_y);
int GetHexFromScreenY(int screen_x, int screen_y);
