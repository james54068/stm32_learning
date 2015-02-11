#ifndef __FILE_COMMAND_H
#define __FILE_COMMAND_H

#include "ff.h"
#include "stm32f4xx.h"
uint8_t ls(uint8_t *Directory);
uint8_t ls_all(uint8_t *Directory);
uint8_t read_file(uint8_t *Directory);
uint8_t write_file(void);

#endif