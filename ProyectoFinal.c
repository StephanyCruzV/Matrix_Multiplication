#include<stdio.h>
#include <omp.h>
#include <unistd.h>
#include<stdlib.h>
#include <time.h>
#include <immintrin.h>

#pragma warning(disable:4996)

int main()
{
    //Variables
    int rowA, colA,rowB, colB;
    int sizeA = 0, sizeB = 0, sizeC = 0;

    //Files
    FILE* FileA;
    FILE* FileB;
    FILE* FileC;

    //Read files
    FileA = fopen("matrizA.txt", "r");
    FileB = fopen("matrizB.txt", "r");
    FileC = fopen("matrizC.txt", "w+");

    //Check if there files could be open
    if (FileA == NULL) {
        printf("Couldn't open file matrizA.");
        return 1;
    }
    if (FileB == NULL) {
        printf("Couldn't open file matrizB.");
        return 1;
    }
    if (FileC == NULL) {
        printf("Couldn't open file matrizC.");
        return 1;
    }

    //Request the matrices sizes
    printf("Matrix A rows\n");
    scanf("%d", &rowA);
    printf("Matrix A columns\n");
    scanf("%d", &colA);
    printf("Matrix B rows\n");
    scanf("%d", &rowB);
    printf("Matrix B columns\n");
    scanf("%d", &colB);

    //Check if matrices are compatible for multiplication
    if (colA != rowB)
    {
        printf("Matrices not compatible for multiplication\n");
        exit(EXIT_SUCCESS);
    }

    //Get size of matrices
    sizeA = rowA * colA;
    sizeB = rowB * colB;

    //Align the space to store the data
    double* A = (double*)aligned_alloc(256,colA * rowA * sizeof(double));
    if (A == NULL)
    {
        printf("Not sufficent memory\n");
        free(A);
        exit(EXIT_SUCCESS);
    }


    double* B = (double*)aligned_alloc(256, colB * rowB * sizeof(double));
    if (B == NULL)
    {
        printf("Not sufficent memory\n");
        free(A);
        free(B);
        
        exit(EXIT_SUCCESS);
    }


    double* C = (double*)aligned_alloc(256, colB * rowA * sizeof(double));
    if (C == NULL)
    {
        printf("Not sufficent memory\n");
        free(A);
        free(B);
        free(C);
        exit(EXIT_SUCCESS);
    }

    double* C_omp = (double*)aligned_alloc(256, colB * rowA * sizeof(double));
    if (C == NULL)
    {
        printf("Not sufficent memory\n");
        free(A);
        free(B);
        free(C);
        free(C_omp);
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
    

    //Secuential
    clock_t start_t_secuencial[5] = { 0 };
    clock_t end_t_secuencial[5] = { 0 };
    #pragma loop (no vector)
    for (int a = 0; a < 5; a++) {
        start_t_secuencial[a] = clock();
        for (int i = 0; i < rowA; i++)
        {
            for (int j = 0; j < colB; j++)
            {
                C[i * colB + j] = 0;
                for (int k = 0; k < rowB; k++)
                    C[i * colB + j] += (A[i * colA + k] * B[j * rowB + k]);
            }
        }
        end_t_secuencial[a] = clock();
    }

    //OpenMp
    clock_t start_t_open[5] = { 0 };
    clock_t end_t_open[5] = { 0 };
    int it=0;
    it=(rowA*colB)/16;
    for (int a = 0; a < 5; a++) {
        start_t_open[a] = clock();
        #pragma omp parallel num_threads(16)
       {
	    int idx,idy,lx,ly;
	    int nextJump;
   	    int thread;

	    thread=omp_get_thread_num();
            idx=(it*thread)/colB;
            idy=(it*thread)%colB;

            nextJump=it*(thread+1);
	    
	    lx=(nextJump/colB);
            if(nextJump%colB==0){
                ly=colB;
            }
            else{
                ly=(nextJump%colB);
            }
            for (int i = idx; i < lx; i++)
            {
		int begJ;
		if(i==idx)
	            begJ=idy;
	        else
                    begJ=0;
                for (int j=begJ; j < colB; j++)
                {
		    if((i+1)==lx && j==ly){
			break;
			}
                    C_omp[i * colB + j] = 0;
                    for (int k = 0; k < rowB; k++)
                        C_omp[i * colB + j] += (A[i * colA + k] * B[j * rowB + k]);
                }
            }
        }
        end_t_open[a] = clock();
    }
    
    //Impresion Matriz Secuencial
    printf("\n Printing Matrix C \n");
    for (int i = 0; i < rowA; i++)
    {
        //printf("\n");
        for (int j = 0; j < colB; j++) {
            //printf(" C: %0.10f ", C[i * colB + j]);
            fprintf(FileC, "%0.10f\n", C_omp[i * colB + j]);
        }
    }

    //Promedio secuencial
    double total_sec = 0;
    double total_t_secuencial[5] = { 0 };
    for (int a = 0; a < 5; a++) {
        total_t_secuencial[a] = ((double)(end_t_secuencial[a] - start_t_secuencial[a])) / CLOCKS_PER_SEC;
        total_sec += total_t_secuencial[a];
	}

    double promedio_sec = total_sec / 5;

    printf("CORRIDA     SERIAL      PARALELO1      PARALELO2 \n");
    for (int i = 0; i < 5; i++)
    {
        printf("%d        %0.8f      %0.8f       %0.8f\n", i, total_t_secuencial[i]);
    }

    printf("PROM:   %0.8f       %0.8f       %0.8f \n", promedio_sec);
    
    printf("PercVsSerial:    -       %0.8f       %0.8f \n");


    fclose(FileA);
    fclose(FileB);
    fclose(FileC);
    free(A);
    free(B);
    free(C);
    

    return 0;

}
