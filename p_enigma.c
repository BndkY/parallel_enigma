#include "p_enigma.h"

void vSetPartitionParams(Params *pParams, int iLength){
    div_t xRotor0;
    div_t xRotor1;

    /* Verif how many times we went through the whole alphabet */
    xRotor0 = div (iLength, ALPHA_LENGTH);

    /* Rotate the remainder times, because the quot is the same letter */
    pParams->pos[0] += xRotor0.rem;
    if (pParams->pos[0]>'Z')
			pParams->pos[0] -= 26;
    
    if (pParams->pos[0] >= notch[pParams->order[0]-1]){
			pParams->pos[1]++;
        }
    
    /* If rotor 0 does a complete turn, it will encounter a notch and move rotor 1 */
    xRotor1 = div (xRotor0.quot, ALPHA_LENGTH);
    pParams->pos[1] += xRotor1.rem;
    if (pParams->pos[1]>'Z')
                pParams->pos[1] -= 26;

    /* Verif if the rotor 1 has reached any notch */
    if (pParams->pos[1] >= notch[pParams->order[1]-1]){
			
            /* Step up both second and third rotors */
			pParams->pos[1]++;
			if (pParams->pos[1]>'Z')
				pParams->pos[1] -= 26;

			pParams->pos[2]++;
			if (pParams->pos[2]>'Z')
				pParams->pos[2] -= 26;
        }
    
    /* If rotor 1 does a complete turn, it will encounter a notch and move rotors 1 and 2 */
    pParams->pos[1] += xRotor1.quot;
    if (pParams->pos[1]>'Z')
				pParams->pos[2] -= 26;
    pParams->pos[2] += xRotor1.quot;
    if (pParams->pos[2]>'Z')
				pParams->pos[2] -= 26;
}

void vSetEnigma(Params *pParams, int iNbMachines){
    /* init machine */
    for(int i=0; i<iNbMachines; i++){
        pParams[i].order[0] = 1;
        pParams[i].order[1] = 2;
        pParams[i].order[2] = 3;

        pParams[i].rings[0] = 'A';
        pParams[i].rings[1] = 'A';
        pParams[i].rings[2] = 'A';

        pParams[i].pos[0] = 'A';
        pParams[i].pos[1] = 'A';
        pParams[i].pos[2] = 'A';

        pParams[i].plug[0] = 'A';
        pParams[i].plug[1] = 'M';
        pParams[i].plug[2] = 'T';
        pParams[i].plug[3] = 'E';
    }
}

void vSeparateNodeData(){

}

void main(){

    Params xEnigmaParams[N_MACHINES_MAX];
    char *pMsg[N_MACHINES_MAX];
    char *pReturnBuff[N_MACHINES_MAX];
    
    int iMsgLength = 0;
    int iNodes = 4;

    char cReturnBuff[MAX_MSG_LENGTH];
    char cMsg[MAX_MSG_LENGTH];
        /* read string from file */
    FILE *f = fopen("enigma.txt", "r");
    fgets(cMsg, MAX_MSG_LENGTH, f);
    fclose(f);

    iMsgLength = strlen(cMsg);

    pMsg[0] = &cMsg[0];
    pMsg[1] = &cMsg[(iMsgLength/iNodes)-1];
    pMsg[2] = &cMsg[(iMsgLength/iNodes)*2-1];
    pMsg[3] = &cMsg[(iMsgLength/iNodes)*3-1];

    pReturnBuff[0] = &cReturnBuff[0];
    pReturnBuff[1] = &cReturnBuff[(iMsgLength/iNodes)-1];
    pReturnBuff[2] = &cReturnBuff[(iMsgLength/iNodes)*2-1];
    pReturnBuff[3] = &cReturnBuff[(iMsgLength/iNodes)*3-1];

    vSetEnigma(&xEnigmaParams[0], iNodes);

    /* Set "second" enigma machine */
    vSetPartitionParams(&xEnigmaParams[1], (iMsgLength/iNodes));
    vSetPartitionParams(&xEnigmaParams[2], (iMsgLength/iNodes)*2);
    vSetPartitionParams(&xEnigmaParams[3], (iMsgLength/iNodes)*3);

    /* encrypt */
    enigma(pMsg[0], pReturnBuff[0], &xEnigmaParams[0]);
    enigma(pMsg[1], pReturnBuff[1], &xEnigmaParams[1]);
    enigma(pMsg[2], pReturnBuff[2], &xEnigmaParams[2]);
    enigma(pMsg[3], pReturnBuff[3], &xEnigmaParams[3]);

    printf("%s\n",pReturnBuff[0]);

    /* Decrypt */
    vSetEnigma(&xEnigmaParams[0], iNodes);
    vSetPartitionParams(&xEnigmaParams[1], (iMsgLength/iNodes));
    vSetPartitionParams(&xEnigmaParams[2], (iMsgLength/iNodes)*2);
    vSetPartitionParams(&xEnigmaParams[3], (iMsgLength/iNodes)*3);
    

    enigma(pReturnBuff[0], pMsg[0], &xEnigmaParams[0]);
    enigma(pReturnBuff[1], pMsg[1], &xEnigmaParams[1]);
    enigma(pReturnBuff[2], pMsg[2], &xEnigmaParams[2]);
    enigma(pReturnBuff[3], pMsg[3], &xEnigmaParams[3]);

    printf("%s\n",pMsg[0]);
}