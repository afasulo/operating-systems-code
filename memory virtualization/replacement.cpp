#include "src.hpp"

// Delete First Frame in linked list
// Return how many frames you stepped through to find replacement frame
int fifo(FrameList* active_frames, FrameList** frame_to_remove_ptr)
{
    if (active_frames == NULL) {
        *frame_to_remove_ptr = NULL;
        return 0;
    }

    // the first frame in the list is the oldest so its the one to remove
    *frame_to_remove_ptr = active_frames;

    return 1;
}

// Delete Least Recently Used Frame
// frame->idx shows how recently frame has been used
// higher number == more recently used
// Return how many frames you stepped through to find replacement frame
int lru(FrameList* active_frames, FrameList** frame_to_remove_ptr)
{
    if (active_frames == NULL) {
        *frame_to_remove_ptr = NULL;
        return 0;
    }

    FrameList* current = active_frames;
    FrameList* lru_frame = current;

    // A lower IDX indicates less recent use
    int min_idx = current -> idx;
    int frames_accessed = 0;


    /* Iterate through all frames to find the one with the lowest 'idx'. */
    while (current != NULL) {
        frames_accessed++;
        if (current->idx < min_idx) {
            min_idx = current->idx;
            lru_frame = current;
        }
        current = current->next;
    }

    *frame_to_remove_ptr = lru_frame;
    return frames_accessed;

}

// Step through active frames, starting with first element in linked list (passed as arg)
// Remove first active frame with frame->clock_bit set to zero
// Reset any clock_bit as you traverse over a frame
// Return how many frames you stepped through to find replacement frame
int clock_lru(FrameList* active_frames, FrameList** frame_to_remove_ptr)
{
    if ( active_frames == NULL) {
        *frame_to_remove_ptr = NULL;
        return 0;
    }

    FrameList* current = active_frames;
    int frames_accessed = 0;

    // continously cycle through frames like the hand on the clock
    while (1) {
        
        frames_accessed++;

        // if the clock_bit is 0 this is the frame to replace
        if(current -> clock_bit == 0) {
            *frame_to_remove_ptr = current;
            return frames_accessed;
        } else {
            current -> clock_bit = 0;
        }
        
        // Advance the pointer
        current = current -> next;

        // wrap around to the begginning if the end of the list is reached
        if (current == NULL) {
            current = active_frames;
        }
    }
}
