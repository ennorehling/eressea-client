#pragma once

#include <stdio.h>

struct cJSON;

struct cJSON *crfile_read(FILE * F, const char *crname);
struct cJSON* crfile_parse(const char* input);
