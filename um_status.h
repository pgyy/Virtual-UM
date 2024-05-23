/**************************************************************
 *
 *                     um_status.h
 *
 *     Virtual UM 
 *     Authors:  Kevin Yuan & Susie Li 
 *     Date:     Nov 20, 2023 
 *
 *     summary
 *
 *     um_status.h contains the interface of functions that modify registers
 *     as well as a query loop that reads an opcode of an instruction and
 *     executes corresponding operations by calling helper functions. 
 * 
 *     Also includes struct definition for UM_T, which represents components
 *     of a UM including registers, a program counter, memory segments, and
 *     the number of words (instructions). 
 *
 **************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <bitpack.h>
#include <sys/stat.h>
#include "fmt.h"
#include "operations.h"
#include "segments.h"

typedef struct UM_T *UM_T;

void run_um(char* fp);