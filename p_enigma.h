#include "enigma.h"
#include <stdlib.h>
#include <omp.h> 

#define MAX_MSG_LENGTH  256
#define ALPHA_LENGTH  26
#define N_MACHINES_MAX  8

void vSetPartitionParams(Params *pParams, int iLength);

extern int notch[5];