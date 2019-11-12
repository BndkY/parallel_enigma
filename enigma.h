/* enigma simulation and bombe, harald schmidl, april 1998
  the encoding scheme uses code from Fauzan Mirza's
  3 rotor German Enigma simulation
  Written by  */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MSGLEN 80
#define TO 'E'
#define DEBUG_ON 1
#define DEBUG_FULL_ON 0

/* Encryption parameters follow */

typedef struct P
{
  char order[3];/*={ 1, 2, 3 };*/
  char rings[3];/*={ 'A','A','A' };*/
  char pos[3];/*={ 'A','A','A' };*/
  char plug[10];/*="AMTE";*/
} Params;

/* Functions */

char scramble(char c, Params *p);
void enigma(char *in, char *out, Params *p);