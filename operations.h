/**************************************************************
 *
 *                     operations.h
 *
 *     Virtual UM 
 *     Authors:  Kevin Yuan & Susie Li
 *     Date:     Nov 20, 2023 
 *
 *     summary
 *
 *     operations.h contains interfaces of non-memory related 
 *     instructions, including conditional move, arithmetic operations 
 *     (add, multiplication, division), bitwise NAND, input/output, 
 *     and load value. 
 *
 **************************************************************/
#include <stdint.h>
#include <stdio.h>

extern void um_cond_mov(uint32_t* ra, uint32_t* rb, uint32_t* rc);

extern void um_add(uint32_t* ra, uint32_t* rb, uint32_t* rc);

extern void um_mult(uint32_t* ra, uint32_t* rb, uint32_t* rc);

extern void um_divide(uint32_t* ra, uint32_t* rb, uint32_t* rc);

extern void um_nand(uint32_t* ra, uint32_t* rb, uint32_t* rc);

extern void um_output(uint32_t* rc);

extern void um_input(FILE* fp, uint32_t* rc);

extern void um_load_val(uint32_t* ra, uint32_t word);