#include "p_enigma.h"

/*take a string, return encoded string*/
void pEnigma(char *in, char *out,size_t ulSize, Params *p)
{
  size_t j;
#if DEBUG_ON
	printf("[PEnigma]Partition size %lu\n", (size_t) ulSize);
#endif
  for(j = 0; j < ulSize; j++)
  out[j] = scramble(in[j], p);
  out[j] = '\0';
#if DEBUG_ON
	printf("[PEnigma]Final Index %lu\n", (size_t) j);
#endif
}

void vSetPartitionParams(Params *pParams, size_t iLength){
    div_t xRotor0;
    div_t xRotor1;

#if DEBUG_ON
    printf("[setPart]rcv length: %lu\n", iLength);
#endif

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

void main(){
    double dTime = 0.0;

    Params xEnigmaParams[N_MACHINES_MAX];
    char *pMsg[N_MACHINES_MAX];
    char *pReturnBuff[N_MACHINES_MAX];
    
    size_t iMsgLength = 0;

    char cReturnBuff[MAX_MSG_LENGTH] = {0};
    char cMsg[MAX_MSG_LENGTH] = {0};

    /* read string from file */
    // FILE *f = fopen("~/Documents/enigma.txt", "r");
    FILE *f = fopen("enigma.txt", "r");
    fgets(cMsg, MAX_MSG_LENGTH, f);
    fclose(f);


    omp_set_num_threads(N_MACHINES_MAX);

    iMsgLength = strlen(cMsg);

    int iNodes = N_MACHINES_MAX;
    size_t ulPartSize = iMsgLength/iNodes;

#if DEBUG_ON
    printf("Nb of nodes: %i\nMsg Length %lu\nPart Size %lu\n",iNodes, iMsgLength, ulPartSize);
#endif

    pMsg[0] = &cMsg[0];
    pReturnBuff[0] = &cReturnBuff[0];
    dTime = omp_get_wtime();
    vSetEnigma(&xEnigmaParams[0], iNodes);

    for(int iI = 1; iI<iNodes; iI++){
#if DEBUG_ON
    printf("Start bytes: %lu\n",(ulPartSize*iI-1));
#endif
        pMsg[iI] = &cMsg[(ulPartSize*iI)];
        pReturnBuff[iI] = &cReturnBuff[(ulPartSize*iI)];
    }

    #pragma omp parallel for schedule(auto)
    for (int iI = 1; iI<iNodes; iI++){
        if(ulPartSize*iI >= 200){
            size_t ulBuffer = 0;
            while(ulBuffer < ulPartSize*iI){
                if((ulPartSize*iI-ulBuffer)>=200){
                    vSetPartitionParams(&xEnigmaParams[iI], 200);
                    ulBuffer+=200;
                }else{
                   vSetPartitionParams(&xEnigmaParams[iI], ulPartSize*iI-ulBuffer);
                   ulBuffer=ulPartSize*iI;
                }
            }
        }else
            vSetPartitionParams(&xEnigmaParams[iI], ulPartSize*iI);
    }

#if DEBUG_ON
    for (int iCount = 0; iCount<N_MACHINES_MAX; iCount++){
        printf("[ENCRYPT]State after part set of %d: %c %c %c\n", iCount, xEnigmaParams[iCount].pos[0], xEnigmaParams[iCount].pos[1], xEnigmaParams[iCount].pos[2]);
    }
#endif

    enigma(pMsg[0], pReturnBuff[0], &xEnigmaParams[0]);
    enigma(pMsg[1], pReturnBuff[1], &xEnigmaParams[1]);
    enigma(pMsg[2], pReturnBuff[2], &xEnigmaParams[2]);
    enigma(pMsg[3], pReturnBuff[3], &xEnigmaParams[3]);

    // FILE *fpC = fopen("enigmaOutCrypt.txt", "ab");
    // if (fpC != NULL)
    // {
    //     fputs(pReturnBuff[0], fpC);
    //     fclose(fpC);
    // }

    // #pragma omp parallel for schedule(auto)
    // for (int iI = 0; iI<iNodes; iI++){
    //     char cPVTBuff[MAX_MSG_LENGTH] = {0};

    //     if(iI == iNodes-1){
    //         enigma(pMsg[iI],  &cPVTBuff[0], &xEnigmaParams[iI]);
    //     }else
    //     {
    //         pEnigma(pMsg[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
    //     }

    //     strcpy(pReturnBuff[iI], &cPVTBuff[0]);
    // }



    //     char cPVTBuff[MAX_MSG_LENGTH] = {0};

//         if (iI != 0){
//             vSetPartitionParams(&xEnigmaParams[iI], ulPartSize*iI);
//             if (iI == (iNodes-1)){
// #if DEBUG_ON
// 	            printf("[pragma for]Partition size %lu\n", (size_t) strlen(pMsg[iI]));
// #endif
//                 enigma(pMsg[iI],  &cPVTBuff[0],(size_t) strlen(pMsg[iI]), &xEnigmaParams[iI]);
//             } else
//             {
//                 enigma(pMsg[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
//             }
//         } else{
//             enigma(pMsg[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
//         }

        // if (iI != 0){
        //     vSetPartitionParams(&xEnigmaParams[iI], ulPartSize*iI);
        //     enigma(pMsg[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
        // } else{
        //     enigma(pMsg[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
        // }

    //     strcpy(pReturnBuff[iI], &cPVTBuff[0]);
    // }

#if DEBUG_ON
    for (int iCount = 0; iCount<N_MACHINES_MAX; iCount++){
        printf("[ENCRYPT]State after encrypt set of %d: %c %c %c\n", iCount, xEnigmaParams[iCount].pos[0], xEnigmaParams[iCount].pos[1], xEnigmaParams[iCount].pos[2]);
    }
#endif
    
    /* Encrypt pragma */
//     # pragma omp parallel
//     {
//         int iID, iNbThreads;
//         iID = omp_get_thread_num();
// #if DEBUG_ON
//         printf("Entering Node: %d\nLength used %i\n",iID, (ulPartSize*iID));
// #endif
//         // if (iID != 0)
//         // {
//         //     vSetPartitionParams(&xEnigmaParams[iID], (ulPartSize*iID));
//         // }
        
//         //Run enigma machine
//         enigma(pMsg[iID],  pReturnBuff[iID], &xEnigmaParams[iID]);
//     }

    //printf("%s\n",pReturnBuff[0]);

    /* Decrypt pragma */
    vSetEnigma(&xEnigmaParams[0], iNodes);

    #pragma omp parallel for schedule(auto)
    for (int iI = 1; iI<iNodes; iI++){
        if(ulPartSize*iI >= 200){
            size_t ulBuffer = 0;
            while(ulBuffer < ulPartSize*iI){
                if((ulPartSize*iI-ulBuffer)>=200){
                    vSetPartitionParams(&xEnigmaParams[iI], 200);
                    ulBuffer+=200;
                }else{
                   vSetPartitionParams(&xEnigmaParams[iI], ulPartSize*iI-ulBuffer);
                   ulBuffer=ulPartSize*iI;
                }
            }
        }else
            vSetPartitionParams(&xEnigmaParams[iI], ulPartSize*iI);
    }

#if DEBUG_ON
    for (int iCount = 0; iCount<N_MACHINES_MAX; iCount++){
        printf("[DECRYPT]State after part set of %d: %c %c %c\n", iCount, xEnigmaParams[iCount].pos[0], xEnigmaParams[iCount].pos[1], xEnigmaParams[iCount].pos[2]);
        }   
#endif


//     # pragma omp parallel
//     {
//         int iID, iNbThreads;
//         char cPVTBuff[MAX_MSG_LENGTH] = {0};
//         // Params xEnigmaParams;
//         iID = omp_get_thread_num();
// #if DEBUG_ON
//         printf("Entering Node: %d\n",iID);
// #endif
//         cPVTBuff[0] = '\n';
//         if(iID != (iNodes-1)){
//             pEnigma(pReturnBuff[iID], &cPVTBuff[1], ulPartSize, &xEnigmaParams[iID]);
//         }else{
//             enigma(pReturnBuff[iID], &cPVTBuff[1], &xEnigmaParams[iID]);
//         }

//         #pragma omp critical
//         for (int iI=0; iI<iNodes; iI++){
//             strcpy(pMsg[iI], &cPVTBuff[0]);
//         }
//     }

    #pragma omp parallel for ordered schedule(static)
    for (int iI = 0; iI<iNodes; iI++){
        char cPVTBuff[MAX_MSG_LENGTH];
        Params xPVTEnigma = xEnigmaParams[iI];
        FILE *fp = fopen("enigmaOut.txt", "ab");

        printf("TESTE NODE FOR NUMERO %i\n", iI);
        if(iI != (iNodes-1)){
            printf("Node interno %i\n", iI);
            pEnigma(pReturnBuff[iI], &cPVTBuff[0], ulPartSize, &xPVTEnigma);
        }else{
            printf("Node final %i\n", iI);
            enigma(pReturnBuff[iI], &cPVTBuff[0], &xPVTEnigma);
        }

        #pragma omp ordered

        if (fp != NULL)
        {
            fputs(&cPVTBuff[0], fp);
            fclose(fp);
        }
        // strcpy(pMsg[iI], &cPVTBuff[0]);
    }

    // pEnigma(pReturnBuff[0], pMsg[0], ulPartSize, &xEnigmaParams[0]);
    // pEnigma(pReturnBuff[1], pMsg[1], ulPartSize, &xEnigmaParams[1]);
    // pEnigma(pReturnBuff[2], pMsg[2], ulPartSize, &xEnigmaParams[2]);
    // enigma(pReturnBuff[3], pMsg[3], &xEnigmaParams[3]);


    // #pragma omp parallel for schedule(auto)
    // for (int iI = 0; iI<iNodes; iI++){
    //     char cPVTBuff[MAX_MSG_LENGTH] = {0};

    //     if(iI == iNodes-1){
    //         enigma(pReturnBuff[iI],  &cPVTBuff[0], &xEnigmaParams[iI]);
    //     }else{
    //         pEnigma(pReturnBuff[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
    //     }
    //     strcpy(pMsg[iI], &cPVTBuff[0]);
    // }


    // #pragma omp parallel for schedule(auto)
    // for (int iI = 0; iI<iNodes; iI++)
    // {
    //     char cPVTBuff[MAX_MSG_LENGTH];

//         if (iI != 0){
//             vSetPartitionParams(&xEnigmaParams[iI], ulPartSize*iI);
//             if (iI == (iNodes-1)){
// #if DEBUG_ON
// 	            printf("[pragma for]Partition size %lu\n", (size_t) strlen(pReturnBuff[iI]));
// #endif
//                 enigma(pReturnBuff[iI],  &cPVTBuff[0],(size_t) strlen(pReturnBuff[iI]), &xEnigmaParams[iI]);
//             } else
//             {
//                 enigma(pReturnBuff[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
//             }
//         } else{
//             enigma(pReturnBuff[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
//         }

    //     if (iI != 0){
    //         vSetPartitionParams(&xEnigmaParams[iI], ulPartSize*iI);
    //         enigma(pReturnBuff[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
    //     } else{
    //         enigma(pReturnBuff[iI],  &cPVTBuff[0], ulPartSize, &xEnigmaParams[iI]);
    //     }

    //     strcpy(pMsg[iI], &cPVTBuff[0]);
    // }


#if DEBUG_ON
    for (int iCount = 0; iCount<N_MACHINES_MAX; iCount++){
        printf("[DECRYPT]State after decrypt set of %d: %c %c %c\n", iCount, xEnigmaParams[iCount].pos[0], xEnigmaParams[iCount].pos[1], xEnigmaParams[iCount].pos[2]);
        }   
#endif

    //printf("%s\n",pMsg[0]);
    dTime =dTime - omp_get_wtime();
    /* Write output to file */
    // FILE *fp = fopen("enigmaOut.txt", "ab");
    // if (fp != NULL)
    // {
    //     fputs(pMsg[0], fp);
    //     fclose(fp);
    // }
    printf("Runtime %lfs\n", dTime);
}