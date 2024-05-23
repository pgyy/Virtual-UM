/**************************************************************
 *
 *                     segments.h
 *
 *     Virtual UM 
 *     Authors:  Kevin Yuan & Susie Li 
 *     Date:     Nov 20, 2023 
 *
 *     summary
 *
 *     segments.h contains the interfaces of functions for memory management
 *     including initializing a segment, mapping and unmapping a segment,
 *     deallocating memory, duplicating a segment of memory, 
 *     and updating or retrieving a word within a segment of memory. 
 * 
 *     Also includes struct definition for Segments_T, which represents 
 *     segments used by the UM. 
 *
 **************************************************************/
#include <stdint.h>
#include "seq.h"

typedef struct Segments_T *Segments_T;

extern Segments_T initialize_Segments();
extern void free_Segments(Segments_T* Segments);
extern uint32_t map_segment(Segments_T Segments, uint32_t length);
extern void unmap_segment(Segments_T Segments, uint32_t seg_ID);
extern uint32_t get_word(Segments_T Segments, uint32_t seg_ID, uint32_t offset);
extern void set_word(Segments_T Segments, uint32_t seg_ID, 
                     uint32_t offset, uint32_t value);
extern uint32_t duplicate(Segments_T Segments, uint32_t source_ID);
