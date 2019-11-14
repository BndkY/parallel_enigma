#include "enigma.h"
#include <stdlib.h>
#include <omp.h> 

#define MAX_MSG_LENGTH  1500000
#define ALPHA_LENGTH  26
#define N_MACHINES_MAX  1
#define PAD             64

void vSetPartitionParams(Params *pParams, int iLength);

extern int notch[5];