#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <immintrin.h>

/* Compiling just serial command: 
 	/usr/local/gcc9.3/bin/gcc -o Final ProyectoFinal.c
*/

/* Compiling Serial + Autovec command:
   This autovectorize with avx512  
	/usr/local/gcc9.3/bin/gcc -o Final ProyectoFinal.c -O -ftree-vectorize -mavx512f -fopt-info-vec 
*/

#pragma warning(disable:4996)

int main()
{
    //Declare general variables
    int rowA, colA,rowB, colB;
    int sizeA = 0, sizeB = 0, sizeC = 0;

    //Files to be used
    FILE* FileA;
    FILE* FileB;
    FILE* FileC;

    //Read each matrix 
    FileA = fopen("matrizA.txt", "r");
    FileB = fopen("matrizB.txt", "r");
    FileC = fopen("matrizC.txt", "w+");

    //Validate if files can be opened
    if (FileA == NULL) {
        printf("Error: Couldn't open file matrizA.");
        return 1;
    }
    if (FileB == NULL) {
        printf("Error: Couldn't open file matrizB.");
        return 1;
    }
    if (FileC == NULL) {
        printf("Error: Couldn't open file matrizC.");
        return 1;
    }

    //Read A matrix dimensions
    printf("Matrix A rows: \n");
    scanf("%d", &rowA);
    printf("Matrix A columns: \n");
    scanf("%d", &colA);
    //Read B matrix dimensions
    printf("Matrix B rows: \n");
    scanf("%d", &rowB);
    printf("Matrix B columns: \n");
    scanf("%d", &colB);

    //Check if matrices are compatible for multiplication
    if (colA != rowB)
    {
        printf("Error: Dimensions not compatible for multiplication\n");
        exit(EXIT_SUCCESS);
    }

    //Get size of matrices
    sizeA = rowA * colA;
    sizeB = rowB * colB;

    //Align memory for matrix data A
    double* A = (double*)aligned_alloc(256,colA * rowA * sizeof(double));
    if (A == NULL)
    {
        printf("Error: Not sufficent memory\n");
        free(A);
        exit(EXIT_SUCCESS);
    }

    //Align memory for matrix data A
    double* B = (double*)aligned_alloc(256, colB * rowB * sizeof(double));
    if (B == NULL)
    {
        printf("Error: Not sufficent memory\n");
        free(A);
        free(B);
        
        exit(EXIT_SUCCESS);
    }

    //Align memory for matrix data A
    double* C = (double*)aligned_alloc(256, colB * rowA * sizeof(double));
    if (C == NULL)
    {
        printf("Error: Not sufficent memor\n");
        free(A);
        free(B);
        free(C);
        exit(EXIT_SUCCESS);
    }

    //Fill matrix A with the data from file
    double auxA = 0, contA=0;

    for (int i = 0; i < rowA; i++) {
        for (int j = 0; j < colA; j++)
        {
            
            if (fscanf(FileA, "%lf", &auxA) == 1)
            {
                A[i*colA+j] = auxA;
                contA++;
                //  printf("A[%d][%d] = %0.11f\n", i, j, A[i * colA + j]);
            }
            else
            {
                //If file couldn't be read, break the loop
                break;
            }
            
        }
    }
	
    printf("%d %d",contA,sizeA);
    //Exit if the file doesn't have the enough data to fill the desired matrix
    if (contA!=sizeA)
    {
        printf("Not sufficent data to fill Matrix A\n");
        free(A);
        exit(EXIT_SUCCESS);
    }

    printf("\n");

    //Fill matrix B with the data from file
    double auxB=0, contB=0;

    for (int i = 0; i < rowB; i++) {
        for (int j = 0; j < colB; j++)
        {
            if (fscanf(FileB, "%lf", &auxB) == 1)
            {
                B[j*rowB+i] = auxB;
                //printf("B[%d][%d] = %0.11f\n", j, i, B[j * filasB + i]);
                contB++;
            }
            else
            {
                //If file couldn't be read, break the loop
                break;
            }
        }
    }

    //Exit if the file doesn't have the enough data to fill the desired matrix
    if (contB!=sizeB)
    {
        printf("Not sufficent data to fill Matrix B\n");
        free(A);
        free(B);
        exit(EXIT_SUCCESS);
    }
    
    /* *********************************************************
                START SECUENTIAL PROCESS
    ********************************************************** */
    clock_t start_t_secuencial[5] = { 0 };
    clock_t end_t_secuencial[5] = { 0 };

    for (int a = 0; a < 5; a++) {
        start_t_secuencial[a] = clock();
        for (int i = 0; i < rowA; i++)
        {
            for (int j = 0; j < colB; j++)
            {
                C[i * colB + j] = 0;
                for (int k = 0; k < rowB; k++){
                    C[i * colB + j] = C[i * colB + j] + (A[i * colA + k] * B[j * rowB + k]);
		    __asm__("nop"); 
            	}   
	    }
        }
        end_t_secuencial[a] = clock();
    }
    
    //Save C matrix calculated with serial process
    printf("\n Printing Matrix C ...\n");
    for (int i = 0; i < rowA; i++)
    {
        //printf("\n");
        for (int j = 0; j < colB; j++) {
            //printf(" C: %0.10f ", C[i * colB + j]);
            fprintf(FileC, "%0.10f\n", C[i * colB + j]);
        }
    }
    /* ******* END SECUENTIAL PROCESS ******* */

    //Secuential mean (calculo del promedio)
    double total_sec = 0;
    double total_t_secuencial[5] = { 0 };
    for (int a = 0; a < 5; a++) {
        total_t_secuencial[a] = ((double)(end_t_secuencial[a] - start_t_secuencial[a])) / CLOCKS_PER_SEC;
        total_sec += total_t_secuencial[a];
	}
    
    double promedio_sec = total_sec / 5;

    /* *********************************************************
                START AUTO VECTORIZATION PROCESS
    ********************************************************** */
    clock_t start_t_int[5] = { 0 };
    clock_t end_t_int[5] = { 0 };

    for (int a = 0; a < 5; a++) {
        start_t_int[a] = clock();
        for (int i = 0; i < rowA; i++)
        {
            for (int j = 0; j < colB; j++)
            {
                C[i * colB + j] = 0;
                for (int k = 0; k < rowB; k++){
                    C[i * colB + j] = C[i * colB + j] + (A[i * colA + k] * B[j * rowB + k]);
            	}   
	    }
        }
        end_t_int[a] = clock();
    }
    
    //Save C matrix calculated with serial process
    printf("\n Comparing Matrix C with AutoVec results: \n");
    	// Add comparation function
    printf("\n ");

    //Intrinsics average (calculo del promedio)
    double total_int = 0;
    double total_t_int[5] = { 0 };
    for (int a = 0; a < 5; a++) {
        total_t_int[a] = ((double)(end_t_int[a] - start_t_int[a])) / CLOCKS_PER_SEC;
        total_int += total_t_int[a];
	}
    
    double promedio_int = total_int / 5;
    
    /* ******* END AUTO  VECTORIZATION PROCESS ******* */

    
    printf("CORRIDA       SERIAL          AUTOVEC        PARALELO2 \n");
    for (int i = 0; i < 5; i++)
    {
        printf("    %d        %0.8f      %0.8f       %0.8f\n", i+1, total_t_secuencial[i], total_t_int[i], total_t_int[i]);
    }
    
    printf("********************************************************* \n");
    printf("PROM:        %0.8f      %0.8f       %0.8f \n", promedio_sec, promedio_int, promedio_int);
    
    printf("\n BEST OPTIMIZATION:\n ");
    printf("  compare proms to get result\n ");
    printf("\n ");

    fclose(FileA);
    fclose(FileB);
    fclose(FileC);
    free(A);
    free(B);
    free(C);

    return 0;

}