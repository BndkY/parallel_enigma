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
    
    /* init all the machines with the same base configuration */
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

    Params xEnigmaParams[N_MACHINES_MAX];   //Passar pra dentro da paralelizacao
    char *pMsg[N_MACHINES_MAX];
    char *pReturnBuff[N_MACHINES_MAX];
    
    int iMsgLength = 0;

    char cReturnBuff[MAX_MSG_LENGTH];
    char cMsg[MAX_MSG_LENGTH] = "uNoyDUYiFUwgWOibZXroPBBBZNcNpPVELtjXyQGzXromwgvFtiVdhmPiGvzObgMAADfXqiAYuDlGalIDHUStWGdwvAcoahaLawNGvWGEPtixbKzbDUdNbRpbXrPDnMZB";

    omp_set_num_threads(N_MACHINES_MAX);

    iMsgLength = strlen(cMsg);

    int iNodes = omp_get_num_threads();
    // int iNodes = 2;

#if DEBUG_ON
    printf("Nb of nodes: %d\n",iNodes);
#endif

    pMsg[0] = &cMsg[0];
    pMsg[1] = &cMsg[(iMsgLength/iNodes)-1];

    pReturnBuff[0] = &cReturnBuff[0];
    pReturnBuff[1] = &cReturnBuff[(iMsgLength/iNodes)-1];

    /* Encrypt pragma */
    vSetEnigma(&xEnigmaParams[0], iNodes);

    # pragma omp parallel
    {
        //put xEnigmaParams inside here 
        int iID, iNbThreads;

        iID = omp_get_thread_num();
        iNbThreads = omp_get_num_threads();
#if DEBUG_ON
        printf("Entering Node: %d\n",iID);
#endif
        /* Set "iID" enigma machine */
        if (iID != 0){
            vSetPartitionParams(&xEnigmaParams[iID], (iMsgLength/iNbThreads));
        }

        /* pragma barrier? */
        // #pragma omp barrier
        //Run enigma machine
        enigma(pMsg[iID], pReturnBuff[iID], &xEnigmaParams[iID]);
    }

    // /* Set "second" enigma machine */
    // vSetPartitionParams(&xEnigmaParams[1], (iMsgLength/iNodes));

    // /* encrypt */
    // enigma(pMsg[0], pReturnBuff[0], &xEnigmaParams[0]);
    // enigma(pMsg[1], pReturnBuff[1], &xEnigmaParams[1]);

    printf("%s\n",pReturnBuff[0]);

    /* Decrypt pragma */
    vSetEnigma(&xEnigmaParams[0], iNodes);

    # pragma omp parallel
    {
        int iID, iNbThreads;

        iID = omp_get_thread_num();
        iNbThreads = omp_get_num_threads();

        /* Set "iID" enigma machine */
        if (iID != 0){
            vSetPartitionParams(&xEnigmaParams[iID], (iMsgLength/iNbThreads));
        }

        //Run enigma machine
        enigma(pReturnBuff[iID], pMsg[iID], &xEnigmaParams[iID]);
    }

    // vSetPartitionParams(&xEnigmaParams[1], (iMsgLength/iNodes));

    // enigma(pReturnBuff[0], pMsg[0], &xEnigmaParams[0]);
    // enigma(pReturnBuff[1], pMsg[1], &xEnigmaParams[1]);

    printf("%s\n",pMsg[0]);
}