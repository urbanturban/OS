#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int page;       // page stored in this memory frame
    int time;       // Time stamp of page stored in this memory frame
    int free;       // Indicates if frame is free or not
                    // Add own data if needed for FIFO, OPT, LFU, Own algorithm
} frameType;

//---------------------- Initializes by reading stuff from file and inits all frames as free -----------------------------------------------------------

void initilize (int *no_of_frames, int *no_of_references, int refs[], frameType frames[]) {

    int i;
    FILE *fp;
    char fileName[50]="/home/student/Desktop/Labs_material/ref.txt";

    fp = fopen(fileName, "r");

    if(fp == NULL) {
        printf("Failed to open file %s", fileName);
        exit(-1);
    }

    fscanf(fp,"%d", no_of_frames);                  //Get the number of frames

    fscanf(fp,"%d", no_of_references);              //Get the number of references in the reference string

    for(i = 0; i < *no_of_references; ++i) {        // Get the reference string
        fscanf(fp,"%d", &refs[i]);
    }
    fclose(fp);

    for(i = 0; i < *no_of_frames; ++i) {
        frames[i].free = 1;                         // Indicates a free frame in memory
    }

    printf("\nPages in memory:\t");                 // Print header with frame numbers
    for(i = 0; i < *no_of_frames; ++i) {
        printf("\t%d", i);
    }
    printf("\n");
}

//-------------------- Prints the results of a reference,  all frames and their content and some info if page fault -----------------------------------

void printResultOfReference (int no_of_frames, frameType frames[], int pf_flag, int mem_flag, int pos, int mem_frame, int ref) {

    int j;

    printf("Acessing page %d:\t", ref);

    for(j = 0; j < no_of_frames; ++j) {             // Print out what pages are in memory, i.e. memory frames
        if (frames[j].free == 0) {                  // Page is in memory
            printf("\t%d", frames[j].page);
        }
        else {
            printf("\t ");
        }
    }

    if(pf_flag == 0) {                              // Page fault
        printf("\t\tPage fault");
    }
    if (mem_flag == 0) {                            // Did not find a free frame
        printf(", replaced frame: %d", pos);
    }
    else if (mem_frame != -1) {                     // A free frame was found
        printf(", used free frame %d", mem_frame);
    }
    printf("\n");
}

//----------- Finds the position in memory to evict in case of page fault and no free memory location ---------------------------------------------

int findPageToEvict(frameType frames[], int n) {   // LRU eviction strategy -- This is what you are supposed to change in the lab for LFU and OPT
    int i, minimum = frames[0].time, pos = 0;

    for(i = 1; i < n; ++i) {
        if(frames[i].time < minimum){               // Find the page position with minimum time stamp among all frames
            minimum = frames[i].time;
            pos = i;
        }
    }
    return pos;                                     // Return that position
}

//---- Main loops ref string, for each ref 1) check if ref is in memory, 2) if not, check if there is free frame, 3) if not, find a page to evict --
int main()
{
    int no_of_frames, no_of_references, refs[100], counter = 0, page_fault_flag, no_free_mem_flag, i, j, pos = 0, faults = 0, free = 0;
    frameType frames[20];

    initilize (&no_of_frames, &no_of_references, refs, frames); // Read no of frames, no of refs and ref string from file

    for(i = 0; i < no_of_references; ++i) {         // Loop over the ref string and check if refs[i] is in memory or not
        page_fault_flag = no_free_mem_flag = 0;     // If not, we have a page fault, and either have a free frame or evict a page

        for(j = 0; j < no_of_frames; ++j) {         // Check if refs[i] is in memory
            if(frames[j].page == refs[i]) {         // Accessed ref is in memory
                counter++;
                frames[j].time = counter;           // Update the time stamp for this frame
                page_fault_flag = no_free_mem_flag = 1; // Indicate no page fault (no page fault and no free memory frame needed)
                free = -1;                          // Indicate no free mem frame needed (reporting purposes)
                break;
            }
        }

        if(page_fault_flag == 0) {                   // We have a page fault
            for(j = 0; j < no_of_frames; ++j) {     // Loop over memory
                if(frames[j].free == 1) {            // Do we have a free frame
                    counter++;
                    faults++;
                    frames[j].page= refs[i];        // Update memory frame with referenced page
                    frames[j].time = counter;       // Update the time stamp for this frame
                    frames[j].free = 0;             // This frame is no longer free
                    no_free_mem_flag = 1;           // Indicate that we do not need to replace since free frame was found
                    free = j;                       // Inicate that we found position j as free (reporting purposes)
                    break;
                }
            }
        }

        if(no_free_mem_flag == 0) {                 // Page fault and memory is full, we need to know what page to evict
            pos = findPageToEvict(frames, no_of_frames); // Get memory position to evict among all frames
            counter++;
            faults++;
            frames[pos].page = refs[i];             // Update memory frame at position pos with referenced page
            frames[pos].time = counter;             // Update the time stamp for this frame
        }
        printResultOfReference (no_of_frames, frames, page_fault_flag, no_free_mem_flag, pos, free, refs[i]); // Print result of referencing ref[i]
    }
    printf("\n\nTotal Page Faults = %d\n\n", faults);

    return 0;
}
