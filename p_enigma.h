#include "enigma.h"
#include <stdlib.h>
#include <omp.h> 

#define MAX_MSG_LENGTH  2000000
#define ALPHA_LENGTH  26
#define N_MACHINES_MAX  1
#define PAD             64

void vSetPartitionParams(Params *pParams, size_t iLength);
void pEnigma(char *in, char *out,size_t ulSize, Params *p);

extern int notch[5];