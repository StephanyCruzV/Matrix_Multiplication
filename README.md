# Matrix_Multiplication

Authors:    Stephany Judith Cruz Vazquez     |       Marco Aurelio Cepeda Treviño



## DESCRIPCION

  Este proyecto se enfoca en realizar multiplicación de matrices n*m.
  
  Se leen dos archivos con los datos de cada matriz (como se muestran en los archivos matrizA.txt y matrizB.txt) y se realiza el proceso de multiplicación de matrices de forma serial y se crea un archivo nuevo con los valores de la matriz resultante, en este caso matrizC.txt
  
  Se busca poder optimizar los procesos por medio de metodos de paralelización, en este caso compararemos tiempos de ejecucuión del proceso serial contra autovectorización y paralelización con ... 
  
  Para la autovectorización se hara uso de las banderas del compilador, se esta haciendo uso de gcc con autovectorización avx512, además se activa la bandera "-fopt-info-vec" la cual nos permite validar que partes se han autovectorizado.
  
  En cuanto a ...
  
  Para ambos metodos se valida que la matriz obtenida sea la correcta, y muestra un mensaje al hacer dichas validaciones.
  
  El calculo de la matriz se realiza 5 veces, para poder realizar una comparación más precisa de los tiempos de ejecución de cada método, al final se muestra una tabla con los tiempos de ejecución de cada metrodo ppor cada corrida, también se calcula el tiempo de ejecución promedio para cada método y al final nos indica cual fue el proceso más eficiente.

## EJECUCION

  Para poder correr el código es necesario correr el siguiente comando:
    
    /usr/local/gcc9.3/bin/gcc -o Final ProyectoFinal.c -O -ftree-vectorize -mavx512f -fopt-info-vec


### RESTRICCIONES

  Este código esta realizado para correr en un sistema linux el cual soporta un set de intrucciones avx512, de igual forma se recomienda verificar que la version del compilador gcc sea compatible.
