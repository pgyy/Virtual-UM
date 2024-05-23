/**************************************************************
 *
 *                     segments.c
 *
 *     Virtual UM 
 *     Authors:  Kevin Yuan & Susie Li 
 *     Date:     Nov 20, 2023 
 *
 *     summary
 *
 *     segments.c contains implementations of functions for memory management
 *     including initializing a segment, mapping and unmapping a segment,
 *     deallocating memory, duplicating a segment of memory, 
 *     and updating or retrieving a word within a segment of memory. 
 * 
 *     Also includes struct definition for Segments_T, which represents 
 *     segments used by the UM. 
 *
 **************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <mem.h>
#include "segments.h"

/********** struct Segments_T ********
 *
 * Segments_T corresponds to the segments of memory, which store instructions
 * to be executed. Specifically, each component corresponds to the following
 * data representation:
 * 
 * Seq_T segments: a sequence of sequences, which store instructions in words 
 * Seq_T unmapped_ids: a sequence of unmapped IDs, which can be used when a new
 *                     memory segment is mapped.
 * 
 *****************************/
struct Segments_T {
        Seq_T     segments;        /* Sequence of sequence of instructions */
        Seq_T     unmapped_ids;    /* Sequence of recycled IDs (uint32_t) */
};

/********** initialize_Segments ********
 *
 * Allocate memory for a Segments_T struct and returns it 
 *
 * Parameters: None
 * Return: 
 *      A newly created Segments_T
 *
 * Expects:
 *      There is enough heap space to allocate 
 *
 * Notes:
 * 	Will CRE if heap memory for new_segments is not successfully allocated 
 *****************************/
extern Segments_T initialize_Segments()
{
        Segments_T new_segments = ALLOC(sizeof(struct Segments_T));
        assert(new_segments);
        new_segments->segments = Seq_new(10);
        new_segments->unmapped_ids = Seq_new(10);
        return new_segments;
}

/********** free_Segments ********
 *
 * Deallocates heal memory allocated for segments 
 * Reuse an available ID if able
 *
 * Parameters: 
 * 	uint32_t length: length of the segment for memory allocation 
 *  	Segments_T segments: modify segments to add one with given length
 * Return: 
 * 	segment ID
 *
 * Expects:
 * 	There is enough heap space to allocate 
 *
 * Notes:
 * 	Will CRE if input_um is NULL
 *****************************/
extern void free_Segments(Segments_T* Segments)
{
        assert(Segments != NULL && *Segments != NULL);
        Seq_T all_segments = (*Segments)->segments;

        /* Loop through all segments and free nonnull instruction segments */
        for (int i = 0; i < Seq_length(all_segments); i++) {
                Seq_T curr = Seq_get(all_segments, i);
                if (curr != NULL) {
                        Seq_free(&curr);
                }
        }

        /* Free any remaining memory */
        Seq_free(&((*Segments)->segments));
        Seq_free(&((*Segments)->unmapped_ids));
        free(*Segments);
}

/********** map_segment ********
 *
 * Allocates a new segment of memory with the specified length and initializes 
 * all values to 0. Attaches the newly mapped segment to the back of the 
 * sequence of segments and reuses an unmapped ID if available.
 *
 * Parameters: 
 * 	uint32_t length: length of the segment for memory allocation 
 *  	Segments_T segments: sequence of segments to be added to 
 * Return: 
 * 	uint32_t map_id: a unique identifier for the newly mapped segment 
 *
 * Expects:
 * 	Segments must not be null
 *
 * Notes:
 * 	Will CRE if Segments is NULL
 *****************************/
extern uint32_t map_segment(Segments_T Segments, uint32_t length)
{
        assert(Segments != NULL);
        Seq_T new_segment = Seq_new(length);
        Seq_addhi(new_segment, (void*)(uintptr_t)0);

        /* Initialize each word in the new segment to 0 */
        for (uint32_t i = 1; i < length; i++) {
                Seq_addhi(new_segment, (void*)(uintptr_t)0);
        }

        uint32_t map_id;
        /* check if there are IDs available in unmapped_id */
        if (Seq_length(Segments->unmapped_ids) > 0) {
                map_id = (uint32_t)(uintptr_t)
                          Seq_remhi(Segments->unmapped_ids);
                Seq_put(Segments->segments, map_id, new_segment);
        } else {
                map_id = (uint32_t)Seq_length(Segments->segments);
                Seq_addhi(Segments->segments, new_segment);
        }
        return map_id;
}

/********** unmap_segment ********
 *
 * Frees a segment of memory based on a segment ID and recycles the ID
 *
 * Parameters: 
 *      Segments_T segments: Where the unmap happens
 *      uint32_t seg_ID: The identifier of the segment to be deallocated.

 * Return: None
 *
 * Expects:
 *      Segments must not be null 
 *      seg_ID must be within range, ie. less than the total number of segments 
 *
 * Notes:
 *      Will CRE if Segments is null 
 *      Will CRE if seg_ID is out of range i.e. greater than number of segments 
 *****************************/
extern void unmap_segment(Segments_T Segments, uint32_t seg_ID)
{
        assert(Segments != NULL);
        assert(seg_ID < (uint32_t)Seq_length(Segments->segments));

        /* Retrieve and free segment of instructions at target ID */
        Seq_T instructions = Seq_get(Segments->segments, seg_ID);
        assert(instructions != NULL);
        Seq_free(&instructions);
        Seq_put(Segments->segments, seg_ID, NULL);

        /* Recycle unmapped ID */
        Seq_addhi(Segments->unmapped_ids, (void*)(uintptr_t)seg_ID);
}

/********** get_word ********
 *
 * Extract a word from a given segment ID and offset within the segment
 *
 * Parameters: 
 *      Segments_T segments: segment from which the words are extracted
 *      uint32_t seg_ID: identifier of the segment to be accessed.
 *      uint32_t offset: index of the word within the segment of instructions
 *  	
 * Return: the word at seg_id and offset 
 *
 * Expects:
 *      - Segments must not be null
 *      - source ID must be within valid index range, ie. less than the number 
 *        of segments 
 *      - offset must be a valid index, ie. less the length of the segment 
 *
 * Notes:
 *      - Will CRE if Segments is null
 *      - Will CRE if source_ID is greater than the number of segments or 
 *        if offset greater than segment length
 *****************************/
extern uint32_t get_word(Segments_T Segments, uint32_t seg_ID, uint32_t offset)
{
        assert(Segments != NULL);
        assert(seg_ID < (uint32_t)Seq_length(Segments->segments));
        assert(offset < (uint32_t)Seq_length(Seq_get(Segments->segments, 
               seg_ID)));
        
        return (uintptr_t)Seq_get(Seq_get(Segments->segments, (int)(seg_ID)), 
                                  (int)offset);
}       

/********** set_word ********
 *
 * Sets a word in the specified segment at the given offset to a given value.
 *
 * Parameters: 
 * 	Segments_T segments: memory where word is stored
 * 	uint32_t seg_ID: a unique segment identifier
 *      uint32_t offset: the index of the word within a segment 
 *      uint32_t value: the value to be stored in the word 
 *
 * Return: none
 *
 * Expects:
 *      - Segments must not be null
 *      - source ID must be within valid index range, ie. less than the number 
 *        of segments 
 *      - offset must be a valid index, ie. less the length of the segment 
 *
 * Notes:
 *      - Will CRE if Segments is null
 *      - Will CRE if source_ID is greater than the number of segments or 
 *        if offset greater than segment length
 *****************************/
extern void set_word(Segments_T Segments, uint32_t seg_ID, uint32_t offset,
                     uint32_t value)
{
        assert(Segments != NULL);
        assert(seg_ID < (uint32_t)Seq_length(Segments->segments));
        assert(offset < (uint32_t)Seq_length(Seq_get(Segments->segments, 
               seg_ID)));

        Seq_put(Seq_get(Segments->segments, (int)(seg_ID)), (int)offset, 
                (void*)(uintptr_t)value);
}

/********** duplicate ********
 *
 * Create a segment identical to the source segment 
 *
 * Parameters: 
 *      Segments_T segments: where the duplication takes place
 *      uint32_t source_ID: positions of the source segment   
 *  	
 * Return: uint32_t size of the duplicated segment
 *
 * Expects:
 *      - Segments must not be null
 *      - source ID must be within valid index range, ie. less than the number 
 *        of segments 
 *      - offset must be a valid index, ie. less the length of the segment 
 *
 * Notes:
 *      - Will CRE if source_ID is greater than the number of segments or 
 *        if offset greater than segment length
 *****************************/
extern uint32_t duplicate(Segments_T Segments, uint32_t source_ID)
{
        assert(Segments != NULL);
        assert(source_ID < (uint32_t)Seq_length(Segments->segments));

        /* If source segment is segment 0, return the length of segment 0 */
        if (source_ID == 0) {
                return Seq_length(Seq_get(Segments->segments, 0));
        }
        Seq_T words = Seq_new(256);

        /* Get previous segment 0 */
        Seq_T source_seg = Seq_get(Segments->segments, source_ID); 
        for (int i = 0; i < Seq_length(source_seg); i++) {
                Seq_addhi(words, Seq_get(source_seg, i));
        }
        
        Seq_T seg0 = Seq_get(Segments->segments, 0);
        Seq_free(&seg0); /* Deallocate previous segment 0 */
        /* Put new duplicated segment into segment 0 */
        Seq_put(Segments->segments, 0, (void*)words); 
        return Seq_length(words);

}