#ifndef _CONFIG_
#define _CONFIG_

#include "math_parser.h"
#define CONFIG_FILE_NAME "config.bin"
#define DEFAULT_RANGE 384

typedef struct program_config{
	angle_type angle;
	int range;
}program_config;

void get_config_file();
void save_config_file();
void select_config(int choice);

#endif