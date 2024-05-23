#include <stdlib.h>
#include "um_status.h"

int main(int argc, char *argv[])
{
        /* EXIT_FAILURE if given incorrect input format */
        if (argc != 2) {
                exit(EXIT_FAILURE);
        }
        /* Open um, append all instructions to segment 0, close um */
        run_um(argv[1]);
        
        return 0;
}