#include<stdio.h>
#include<stdlib.h>
#include <omp.h>
#include <time.h>
#include <immintrin.h>


/* Compiling Serial + Autovec + OMP command:
   This autovectorize with avx512  
	/usr/local/gcc9.3/bin/gcc -o Final ProyectoFinal.c -O -ftree-vectorize -mavx512f -fopt-info-vec -fopenmp
*/


// Adding color mesagges functions

void colorReset () {
  printf("\033[0m");
}

void setRed () {
  printf("\033[1;31m");
}

void setGreen () {
  printf("\033[0;32m");
}

void setBlue () {
  printf("\033[0;36m");
}


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
	setRed();
        printf("Error: Couldn't open file matrizA.");
	colorReset();
        return 1;
    }
    if (FileB == NULL) {
	setRed();
        printf("Error: Couldn't open file matrizB.");
	colorReset();
        return 1;
    }
    if (FileC == NULL) {
	setRed();
        printf("Error: Couldn't open file matrizC.");
	colorReset();
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
	setRed();
        printf("Error: Dimensions not compatible for multiplication\n");
	colorReset();
        exit(EXIT_SUCCESS);
    }

    //Get size of matrices
    sizeA = rowA * colA;
    sizeB = rowB * colB;

    //Align memory for matrix data A
    double* A = (double*)aligned_alloc(256,colA * rowA * sizeof(double));
    if (A == NULL)
    {
	setRed();
        printf("Error: Not sufficent memory\n");
	colorReset();
        free(A);
        exit(EXIT_SUCCESS);
    }

    //Align memory for matrix data A
    double* B = (double*)aligned_alloc(256, colB * rowB * sizeof(double));
    if (B == NULL)
    {
	setRed();
        printf("Error: Not sufficent memory\n");
	colorReset();
        free(A);
        free(B);
        
        exit(EXIT_SUCCESS);
    }

    //Align memory for matrix data A
    double* C = (double*)aligned_alloc(256, colB * rowA * sizeof(double));
    if (C == NULL)
    {
	setRed();
        printf("Error: Not sufficent memory\n");
	colorReset();
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
	
    //printf("%d %d",contA,sizeA);
    //Exit if the file doesn't have the enough data to fill the desired matrix
    if (contA!=sizeA)
    {
	setRed();
        printf("Error : Not sufficent data to fill Matrix A\n");
	colorReset();
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
	setRed();
        printf("Error: Not sufficent data to fill Matrix B\n");
	colorReset();
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
    printf("\n Serial Process Running ... \n");
    printf("\n     Serial Process  -> ");
    setGreen();
    printf(" Successful \n");
    colorReset();
    printf("\n Saving C Matrix  ... \n");
    printf("\n     matrizC.txt file  -> ");
    setGreen();
    printf(" Successful \n");
    colorReset();

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
    
    //Declare AutoVec C matrix
    double* autoC = (double*)aligned_alloc(256, colB * rowA * sizeof(double));
    if (autoC == NULL)
    {
	    setRed();
        printf("Error: Not sufficent memory\n");
	    colorReset();
        free(A);
        free(B);
        free(C);
        free(autoC);
        exit(EXIT_SUCCESS);
    }
    
    clock_t start_t_int[5] = { 0 };
    clock_t end_t_int[5] = { 0 };

    for (int a = 0; a < 5; a++) {
        start_t_int[a] = clock();
        for (int i = 0; i < rowA; i++)
        {
            for (int j = 0; j < colB; j++)
            {
               autoC[i * colB + j] = 0;
                for (int k = 0; k < rowB; k++){
                   autoC[i * colB + j] = autoC[i * colB + j] + (A[i * colA + k] * B[j * rowB + k]);
            	}   
	    }
        }
        end_t_int[a] = clock();
    }
    
    //Save C matrix calculated with serial process
    printf("\n Comparing Matrix C with AutoVec results ... \n");
    // Add comparation function
    int vecCompare = 1;

    for (int i = 0; i < rowA; i++)
    {
        for (int j = 0; j < colB; j++) {
    	    // Test if comparations is made correctly
            //autoC[i*colB+j] = 2.10;
            if (C[i * colB + j] == autoC[i * colB + j]){
	    	vecCompare = 1;
	    }
	    else {
		vecCompare = 0;
		break;
	    }
        }
    }

    /*
    // Verify auto vectorization results
    for (int i = 0; i < 20; i++){
   	printf("%0.10f          %0.10f \n" , C[i], autoC[i]);
   	//printf("%0.10f \n" , autoC[i]);
    }
    */

    if (vecCompare == 0) {
    	setRed();
        printf("       Error: AutoVectorization Process Failed.\n");
    	colorReset();
        printf("\n ");
        return 1;
    }
    else {
 	printf("\n     Auto Vectorization Process -> ");
    	setGreen();
    	printf(" Successful \n");
    	colorReset();
    }

    printf("\n ");
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

    /* *********************************************************
                START OpenMP PROCESS
    ********************************************************** */
    double* C_omp = (double*)aligned_alloc(256, colB * rowA * sizeof(double));
    if (C_omp == NULL)
    {
        printf("Not sufficent memory\n");
        free(A);
        free(B);
        free(C);
        free(autoC);
        free(C_omp);
        exit(EXIT_SUCCESS);
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

    printf("Comparing Matrix C with OpenMP results ... \n");
    // Add comparation function
    int ompCompare = 1;
    for (int i = 0; i < rowA; i++)
    {
        for (int j = 0; j < colB; j++) {
            if (C[i * colB + j] == C_omp[i * colB + j]){
	    	ompCompare = 1;
	    }
	    else {
		ompCompare = 0;
		break;
	    }
        }
    }


    if (ompCompare == 0) {
    	setRed();
        printf("Error: OpenMP Process Failed.");
    	colorReset();
        return 1;
    }
    else {
 	printf("\n    Open MP Process -> ");
    	setGreen();
    	printf(" Successful \n");
    	colorReset();
    }

    printf("\n ");
    printf("\n ");

    //Promedio openMP
    double total_open = 0;
    double total_t_open[5] = { 0 };
    for (int a = 0; a < 5; a++) {
        total_t_open[a] = ((double)(end_t_open[a] - start_t_open[a])) / CLOCKS_PER_SEC;
        total_t_open[a]=total_t_open[a]/16;
        total_open += total_t_open[a];
    }

    double promedio_open = total_open / 5;
    
    setBlue();
    printf("CORRIDA       SERIAL          AUTOVEC            OMP   \n");
    colorReset();
    for (int i = 0; i < 5; i++)
    {
        printf("    %d        %0.8f      %0.8f       %0.8f\n", i+1, total_t_secuencial[i], total_t_int[i], total_t_open[i]);
    }
    
    printf("********************************************************* \n");
    printf("PROM:        %0.8f      %0.8f       %0.8f \n", promedio_sec, promedio_int, promedio_open);
    printf("\n ");
    
    printf("\n BEST OPTIMIZATION    ->      ");
    //printf("  compare proms to get result\n ");
    setGreen();
    if (promedio_sec > promedio_int || promedio_sec > promedio_open) {
        if (promedio_int > promedio_open)
            printf("OpenMP \n");
        else
            printf("AUTO-VECTORIZATION \n");
    }
    else
    {
        printf("SERIAL");
    }
    colorReset();
    printf("\n ");
    printf("\n ");

    fclose(FileA);
    fclose(FileB);
    fclose(FileC);
    free(A);
    free(B);
    free(C);
    free(autoC);
    free(C_omp);

    return 0;

}
