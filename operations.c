/********************************************************************
 *
 *                     operations.c
 *
 *     Virtual UM
 *     Authors:  Kevin Yuan & Susie Li
 *     Date:     Nov 20, 2023 
 *
 *     summary
 *
 *     operations.c contains implementations of non-memory related 
 *     instructions, including conditional move, arithmetic operations 
 *     (add, multiplication, division), bitwise NAND, input/output, 
 *     and load value. 
 *
 *******************************************************************/
#include <stdint.h>
#include <bitpack.h>
#include <math.h>
#include <assert.h>
#include "operations.h"
#include "fmt.h"

/********** cond_mov ********
 *
 * Moves value in register b into register a if register c value is nonzero 
 *
 * Parameters:
 *      uint32_t *ra: pointer to the register whose value will be modified if 
 *                    the condition is met
 *      uint32_t *rb: pointer to the register which will be copied into ra if 
 *                    the condition is met 
 *      uint32_t *rc: pointer to the register whose value is tested as the 
 *                    condition for move
 *
 * Return: None 
 *
 * Expects:
 *      ra, rb and rc must not be NULL
 * Notes:
 *      Will CRE if ra, rb or rc is NULL
 *      Register values are modified via pointers 
 ************************/
extern void um_cond_mov(uint32_t* ra, uint32_t* rb, uint32_t* rc)
{
        assert(ra != NULL && rb != NULL && rc != NULL);
        if (*rc != 0) { /* Only move rb into ra when rc is nonzero */
                *ra = *rb;
        }
}

/********** add ********
 *
 * Adds the values in registers b and c and stores the result in register a
 *
 * Parameters:
 *      uint32_t *ra: pointer to the register which will store the result of 
 *                    addition
 *      uint32_t *rb: pointer to the first operand of addition 
 *      uint32_t *rc: pointer to the second operand of addition 
 *
 * Return: None 
 *
 * Expects
 *      ra, rb and rc must not be NULL
 * Notes:
 *      Will CRE if ra, rb or rc is NULL
 *      Register values are modified via pointers 
 ************************/
extern void um_add(uint32_t* ra, uint32_t* rb, uint32_t* rc)
{
        assert(ra != NULL && rb != NULL && rc != NULL);
        *ra = (*rb + *rc);
}


/********** mult ********
 * 
 * Multiplies the values in registers b and c and stores the result in reg a
 *
 * Parameters:
 *      uint32_t *ra: pointer to the register which will store the result of 
 *                    multiplication
 *      uint32_t *rb: pointer to the first operand of addition 
 *      uint32_t *rc: pointer to the second operand of addition 
 *
 * Return: None 
 *
 * Expects
 *      ra, rb and rc must not be NULL
 * Notes:
 *      Will CRE if ra, rb or rc is NULL
 *      Register values are modified via pointers 
 ************************/
extern void um_mult(uint32_t* ra, uint32_t* rb, uint32_t* rc)
{
        assert(ra != NULL && rb != NULL && rc != NULL);
        *ra = ((*rb) * (*rc));
}


/********** divide ********
 *
 * Divides the values in registers b by value in register c and stores the 
 * result in register a
 *
 * Parameters:
 *      uint32_t* ra: pointer to the register storing the result of operation
 *      uint32_t* rb: pointer to the first operand of division (numerator)
 *      uint32_t* rc: pointer to the second operand of division (denominator)
 *
 * Return: None 
 *
 * Expects
 *      ra, rb and rc must not be NULL
 * Notes:
 *      Will CRE if ra, rb or rc is NULL
 *      Register values are modified via pointers 
 ************************/
extern void um_divide(uint32_t* ra, uint32_t* rb, uint32_t* rc)
{
        assert(ra != NULL && rb != NULL && rc != NULL);
        *ra = floor(*rb / *rc);
}

/********** um_nand ********
 *
 * Return true if all scores are under a given limit and return number of
 * scores under the limit via reference parameter
 *
 * Parameters:
 *      uint32_t* ra: pointer to the register storing the result of operation 
 *      uint32_t* rb: pointer to the first operand of bitwise NAND 
 *      uint32_t* rc: pointer to the second operand of bitwise NAND 
 * 
 * Return: true if all scores are under limit, false if not
 *
 * Expects
 *      ra, rb and rc must not be NULL
 * Notes:
 *      Will CRE if ra, rb or rc is NULL
 *      Register values are modified via pointers 
 ************************/
extern void um_nand(uint32_t* ra, uint32_t* rb, uint32_t* rc)
{
        assert(ra != NULL && rb != NULL && rc != NULL);
        *ra = ~(*rb & *rc);
}

/********** um_output ********
 *
 * Writes the value in register c to standard output as an ASCII character. 
 * Includes error-handling for out of range values.
 *
 * Parameters:
 *      uint32_t* rc: pointer to the register whose value will be written
 * 
 * Return: None
 *
 * Expects
 *      rc must not be NULL
 *      rc must be less than or equal to 255
 * Notes:
 *      Will CRE if rc is NULL.
 *      Will CRE if the value in rc evaluates to an integer greater than 255. 
 ************************/
extern void um_output(uint32_t* rc)
{
        assert(rc != NULL);
        unsigned int val = *rc;
        assert(val <= 255);
        putc(val, stdout);
}

/********** um_input ********
 *
 * Loads register c with input given via the IO device 
 *
 * Parameters:
 *      uint32_t* rc: pointer to the register whose value will be written
 * 
 * Return: None
 *
 * Expects
 *      fp and rc must not be NULL
 *      input value must be between 0 to 255 inclusive 
 * Notes:
 *      Will CRE if fp or rc is NULL.
 *      Will CRE if input value is not between 0 to 255 
 *      Will fill rc with an all-1 32-bit int if the input is the EOF signal
 ************************/
extern void um_input(FILE* fp, uint32_t* rc)
{
        assert(fp != NULL && rc != NULL);
        int curr_val = fgetc(fp);
        assert(curr_val >= 0 && curr_val <= 255);
        if (curr_val == EOF) {
                *rc = ~0;
        } else {
                *rc = curr_val;
        }
}

/********** um_load_val ********
 *
 * Loads a given value in register a 
 *
 * Parameters:
 *      uint32_t *ra: pointer to the register value which will be modified 
 *      uint32_t word: uint32_t value to be stored in register a 
 *
 * Return: None 
 *
 * Expects
 *      ra must not be NULL
 * Notes:
 *      Will CRE if ra is NULL 
 ************************/
extern void um_load_val(uint32_t* ra, uint32_t word)
{
        assert(ra != NULL);
        *ra = Bitpack_getu(word, 25, 0);
}