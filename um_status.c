/**************************************************************
 *
 *                     um_status.c
 *
 *     Virtual UM 
 *     Authors:  Kevin Yuan & Suzie Li 
 *     Date:     Nov 20, 2023 
 *
 *     summary
 *
 *     um_status.c contains implementations of functions that modify registers
 *     as well as a query loop that reads an opcode of an instruction and
 *     executes corresponding operations by calling helper functions. 
 * 
 *     Also includes struct definition for UM_T, which represents components
 *     of a UM including registers, a program counter, memory segments, and
 *     the number of words (instructions). 
 *
 **************************************************************/
#include "um_status.h"

/************* UM_T struct ********
 * 
 * uint32_t registers[]: array of uint32_t, corresponding to 8 registers 
 * uint32_t pc: program counter, keeps track of the instruction being executed
 * uinted 32_t num_of_word: the number of words added to instruction segment
 * Segments_T Segments: struct representing mapped segments and unmapped IDs
 * 
 *********************************/
struct UM_T {
        uint32_t        registers[8];    /* 8 registers */
        uint32_t        pc;              /* program counter */
        uint32_t        num_of_word;     /* number of words (instructions) */
        Segments_T      Segments;        /* memory segments */
};

bool halted = false; /* Keeps track of whether the input um called halt */

/********** um_map_seg ********
 *
 * Maps a segment of memory with a unique segment ID
 *
 * Parameters:
 *      UM_T um: pointer to register in wh
 *      uint32_t *rb: pointer to register 
 *      uint32_t *rc: pointer to a register value used as the length of the 
 *                    new segment
 *
 * Return: None
 *
 * Expects
 *      um, rb and rc must not be NULL
 * Notes:
 *      Will CRE if um, rb or rc is NULL
 ************************/
static void um_map_seg(UM_T um, uint32_t* rb, uint32_t* rc)
{
        assert(um != NULL && rb != NULL && rc != NULL);
        uint32_t length = *rc;
        *rb = map_segment(um->Segments, length); /* storing new map id to rb */
}

/********** um_unmap_seg ********
 *
 * Frees a segment of memory and recycles its segment ID
 *
 * Parameters:
 *      UM_T um: the UM whose segment will be modified (unmapped)
 *      uint32_t *map_id: the identifier (index) of the segment to be unmapped
 *
 * Return: None
 *
 * Expects
 *      um, rb and rc must not be NULL
 * Notes:
 *      Will CRE if um, rb or rc is NULL
 ************************/
static void um_unmap_seg(UM_T um, uint32_t* map_id)
{
        assert(um != NULL);
        unmap_segment(um->Segments, *map_id);
}

/********** um_get_word ********
 *
 * Returns a word at a specific index in a segment
 * 
 * Parameters:
 *      UM_T um: the UM whose segment will be modified (unmapped)
 *      uint32_t seg_ID: index of the segment in which to get the word from 
 *      uint32_t offset: index of the word in the segment of instructions
 * 
 * Return: 
 *      uint32_t word at segments[seg_ID][offset] 
 *
 * Expects:
 *      um must not be NULL
 * Notes:
 *      Will CRE if um is NULL
 ************************/
static uint32_t um_get_word(UM_T um, uint32_t seg_ID, uint32_t offset)
{
        assert(um != NULL);
        return get_word(um->Segments, seg_ID, offset);
}

/********** um_set_word ********
 *
 * Updates the value of a word at segments[seg_ID][offset]
 * 
 * Parameters:
 *      UM_T um: the UM whose segment will be modified (unmapped)
 *      uint32_t seg_ID: index of the segment in which to get the word from 
 *      uint32_t offset: index of the word in the segment of instructions
 *      uint32_t value: the value which will replace segments[seg_ID][offset]
 * 
 * Return: 
 *      uint32_t word at segments[seg_ID][offset] 
 *
 * Expects:
 *      um must not be NULL
 * Notes:
 *      Will CRE if um is NULL
 ************************/
static void um_set_word(UM_T um, uint32_t seg_ID, uint32_t offset, 
                        uint32_t value)
{
        assert(um != NULL);
        set_word(um->Segments, seg_ID, offset, value);
}

/********** um_seg_load ********
 *
 * Updates the value of a word at segments[seg_ID][offset]
 * 
 * Parameters:
 *      UM_T um: the UM whose segment will be modified (unmapped)
 *      uint32_t seg_ID: index of the segment in which to get the word from 
 *      uint32_t offset: index of the word in the segment of instructions
 *      uint32_t value: the value which 
 * 
 * Return: 
 *      uint32_t word at segments[seg_ID][offset] 
 *
 * Expects:
 *      um must not be NULL
 * Notes:
 *      Will CRE if um is NULL
 ************************/
static void um_seg_load(UM_T um, uint32_t* ra, uint32_t* rb, uint32_t* rc)
{
        assert(um != NULL);
        *ra = um_get_word(um, *rb, *rc);
}

/********** um_seg_store ********
 *
 * Store the value in register c in segments[ra][rb] by calling helper function
 * 
 * Parameters:
 *      UM_T um: the UM whose segment will be modified (unmapped)
 *      uint32_t ra: index of the segment in which to get the word from 
 *      uint32_t rb: index of the target word in the segment of instructions
 *      uint32_t rc: the value which will be stored in segments[ra][rb]
 * 
 * Return: 
 *      uint32_t word at segments[ra][rb] 
 *
 * Expects:
 *      um must not be NULL
 * Notes:
 *      Will CRE if um is NULL
 ************************/
static void um_seg_store(UM_T um, uint32_t* ra, uint32_t* rb, uint32_t* rc)
{
        assert(um != NULL);
        um_set_word(um, *ra, *rb, *rc);
}

/********** um_load_prog ********
 *
 * Duplicates the segment whose ID is rb and replaces segment 0 with it 
 * 
 * Parameters:
 *      UM_T um: the UM whose segment will be modified (unmapped)
 *      uint32_t rc: index of the target word in the segment of instructions
 *      uint32_t rc: the value which will be stored in segments[ra][rb]
 * 
 * Return: None
 *
 * Expects:
 *      um must not be NULL
 * Notes:
 *      Will CRE if um is NULL
 ************************/
static void um_load_prog(UM_T um, uint32_t* rb, uint32_t* rc)
{
        assert(um != NULL);
        um->num_of_word = duplicate(um->Segments, *rb);
        um->pc = *rc;
}

/********** um_halt ********
 *
 * Stops program computation and frees all memory 
 * 
 * Parameters:
 *      UM_T um: the UM whose execution will be halted 
 * 
 * Return: None
 *
 * Expects:
 *      um must not be NULL
 * Notes:
 *      Will CRE if um is NULL
 ************************/
static void um_halt(UM_T um)
{
        assert(um != NULL);
        free_Segments(&(um->Segments));
}

/********** cases ********
 *
 * Call functions to modify register values based on the operation code 
 * 
 * Parameters:
 *      UM_T um: the UM whose registers will be loaded and modified
 *      uint32_t word: an instruction containing opcode and register info
 * 
 * Return: None
 *
 * Expects:
 *      ra, rb, rc, fp, and um must not be NULL
 * Notes:
 *      Will CRE if ra, rb, rc, fp, or um is NULL
 ************************/
static void cases(int opcode, uint32_t* ra, uint32_t* rb, uint32_t* rc, 
                  FILE* fp, UM_T um)
{
        assert(ra != NULL && rb != NULL && rc != NULL && fp != NULL && 
               um != NULL);
        if (opcode == 0) {
                um_cond_mov(ra, rb, rc);
        } else if (opcode == 1) {
                um_seg_load(um, ra, rb, rc);
        } else if (opcode == 2) {
                um_seg_store(um, ra, rb, rc);
        } else if (opcode == 3) {
                um_add(ra, rb, rc);
        } else if (opcode == 4) {
                um_mult(ra, rb, rc);
        } else if (opcode == 5) {
                um_divide(ra, rb, rc);
        } else if (opcode == 6) {
                um_nand(ra, rb, rc);
        } else if (opcode == 7) {
                halted = true;
        } else if (opcode == 8) {
                um_map_seg(um, rb, rc);
        } else if (opcode == 9) {
                um_unmap_seg(um, rc);
        } else if (opcode == 10) {
                um_output(rc);
        } else if (opcode == 11) {
                um_input(fp, rc);
        } else if (opcode == 12) {
                um_load_prog(um, rb, rc);
        }
}

/********** execute_instruction ********
 *
 * Unpack instructions into opcode and registers a, b, c
 * 
 * Parameters:
 *      UM_T um: the UM whose registers will be loaded and modified
 *      uint32_t word: an instruction containing opcode and register info 
 * 
 * Return: None
 *
 * Expects:
 *      um must not be NULL
 * Notes:
 *      Will CRE if um is NULL
 ************************/
static void execute_instruction(UM_T um, uint32_t word) 
{
        assert(um != NULL);
        int opcode = Bitpack_getu(word, 4, 28);
        if (opcode == 13) {
                uint32_t* reg = &(um->registers[Bitpack_getu(word, 3, 25)]);
                um_load_val(reg, word);
        } else if (opcode >= 0 && opcode <= 12) {
                uint32_t* ra = &(um->registers[(int)Bitpack_getu(word, 3, 6)]);
                uint32_t* rb = &(um->registers[(int)Bitpack_getu(word, 3, 3)]);
                uint32_t* rc = &(um->registers[(int)Bitpack_getu(word, 3, 0)]);
                cases(opcode, ra, rb, rc, stdin, um);
        }
}

/********** read_file_to_seg0 ********
 *
 * Reads a file and stores all instructions in segment 0. Includes 
 * error-handling for invalid files. 
 * 
 * Parameters:
 *      char* fp: input .um file containing all instructions to be executed
 * 
 * Return: 
 *      uint32_t: the number of words (instructions) in segment 0
 *
 * Expects:
 *      file_name and um must not be NULL
 * Notes:
 *      Will CRE if fp or um is NULL
 ************************/
static uint32_t read_file_to_seg0(char* file_name, UM_T um) 
{
        assert(file_name != NULL && um != NULL);

        /* Error-handling for invalid input files */
        struct stat s_file;
        if (stat(file_name, &s_file) == -1) {
                fprintf(stderr, "%s: Cannot find this file\n", file_name);
                exit(EXIT_FAILURE);
        }
        size_t file_size = s_file.st_size;
        uint32_t num_of_instruction = file_size / 4;

        FILE *fp = fopen(file_name, "rb");
        assert(fp != NULL);

        /* Allocate memory for segments */
        map_segment(um->Segments, num_of_instruction);

        uint32_t word;
        int curr_byte;
        
        /* Store all instructions as words */
        for (size_t i = 0; i < num_of_instruction; i++) {
                word = 0;
                for (int byte = 0; byte < 4; byte++) {
                        curr_byte = getc(fp);
                        if (curr_byte == EOF) {
                                fclose(fp);
                                return 0; /* Invalid file */
                        }
                        word = (word << 8) | (uint8_t)curr_byte;
                }
                um_set_word(um, 0, i, word);
        }
        fclose(fp);
        return num_of_instruction;
}

/********** run_um ********
 *
 * Initializes a UM and allocates memory for components
 * of the UM including registers and segments, executes all instructions, and
 * frees all memory.
 * 
 * Parameters:
 *      char* fp: input .um file containing all instructions to be executed
 * 
 * Return: None
 *
 * Expects:
 *      fp must not be NULL
 * Notes:
 *      Will CRE if fp is NULL
 *      Heap memory allocated for UM_T um in this function will be freed by 
 *              the caller using an external function 
 ************************/
void run_um(char* fp)
{
        assert(fp != NULL);
        /* Allocate memory for a UM and initialize all components including 
           registers, program counter, and segments */
        UM_T um = malloc(sizeof(struct UM_T));
        assert(um != NULL);
        /* Zero-initialize all registers */
        for (int i = 0; i < 8; i++) {
                um->registers[i] = 0;
        }
        um->pc = 0;
        um->Segments = initialize_Segments();

        /* Fill segment 0 by loading all given instructions */
        um->num_of_word = read_file_to_seg0(fp, um);
        
        /* Execute all instructions by calling corresponding functions */
        while (um->pc < um->num_of_word && !halted) {
                uint32_t instruction = um_get_word(um, 0, (um->pc)++);
                execute_instruction(um, instruction);
        }

        /* Halt program and free all memory */
        um_halt(um);
        free(um);
}