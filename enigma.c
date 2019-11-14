#include "enigma.h"


/* Rotor wirings */
char rotor[5][26]={
	/* Input "ABCDEFGHIJKLMNOPQRSTUVWXYZ" */
	/* 1: */ "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
	/* 2: */ "AJDKSIRUXBLHWTMCQGZNPYFVOE",
	/* 3: */ "BDFHJLCPRTXVZNYEIWGAKMUSQO",
	/* 4: */ "ESOVPZJAYQUIRHXLNFTGKDCMWB",
	/* 5: */ "VZBRGITYUPSDNHLXAWMJQOFECK" };
char ref[26]="YRUHQSLDPXNGOKMIEBFZCWVJAT";

char notch[5]="QEVJZ";


/*take a char and return its encoded version according to the 
  encryption params, update params, i.e. advance wheels
  this part uses Fauzan Mirza's code*/
char scramble(char c, Params *p)
{
  int i, j, flag = 0;

		c=toupper(c);
		if (!isalpha(c))
			return -1;
		
		/* Step up first rotor */
		p->pos[0]++;
		if (p->pos[0]>'Z')
			p->pos[0] -= 26;

		/* Check if second rotor reached notch last time */
		if (flag)
		{
			/* Step up both second and third rotors */
			p->pos[1]++;
			if (p->pos[1]>'Z')
				p->pos[1] -= 26;
			p->pos[2]++;
			if (p->pos[2]>'Z')
				p->pos[2] -= 26;
			flag=0;
		}

		/*  Step up second rotor if first rotor reached notch */
		if (p->pos[0]==notch[p->order[0]-1])
		{
			p->pos[1]++;
			if (p->pos[1]>'Z')
				p->pos[1] -= 26;
			/* Set flag if second rotor reached notch */
			if (p->pos[1]==notch[p->order[1]-1])
				flag=1;
		}

		/*  Swap pairs of letters on the plugboard */
		for (i=0; p->plug[i]; i+=2)
		{
			if (c==p->plug[i])
				c=p->plug[i+1];
			else if (c==p->plug[i+1])
				c=p->plug[i];
		}

		/*  Rotors (forward) */
		for (i=0; i<3; i++)
		{
			c += p->pos[i]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->rings[i]-'A';
			if (c<'A')
				c += 26;

			c=rotor[p->order[i]-1][c-'A'];

			c += p->rings[i]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->pos[i]-'A';
			if (c<'A')
				c += 26;
		}

		/*  Reflecting rotor */
		c=ref[c-'A'];

		/*  Rotors (reverse) */
		for (i=3; i; i--)
		{
			c += p->pos[i-1]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->rings[i-1]-'A';
			if (c<'A')
				c += 26;

			for (j=0; j<26; j++)
				if (rotor[p->order[i-1]-1][j]==c)
					break;
			c=j+'A';

			c += p->rings[i-1]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->pos[i-1]-'A';
			if (c<'A')
				c += 26;
		}
		
		/*  Plugboard */
		for (i=0; p->plug[i]; i+=2)
		{
			if (c==p->plug[i])
				c=p->plug[i+1];
			else if (c==p->plug[i+1])
				c=p->plug[i];
		}
#if DEBUG_FULL_ON
		printf("Enigma params Scramble\n %i, %i, %i\n\n", p->pos[0], p->pos[1], p->pos[2]);
#endif

  return c;
}

/*take a string, return encoded string*/
void enigma(char *in, char *out,size_t ulSize, Params *p)
{
  int j;
#if DEBUG_ON
	printf("[Enigma]Partition size %lu\n", (size_t) ulSize);
#endif
  for(j = 0; j < ulSize; j++)
  out[j] = scramble(in[j], p);
  out[j] = '\0';
}