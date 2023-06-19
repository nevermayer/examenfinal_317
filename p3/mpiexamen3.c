#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define MAX_palabra_SIZE 100

int check_palindrome(char *palabra, int inicio, int fin)
{
    int i, palindromo = 1;
    for (i = inicio; i <= fin; i++)
    {
        if (palabra[i] != palabra[strlen(palabra) - i - 1])
        {
            palindromo = 0;
            break;
        }
    }
    return palindromo;
}

int main(int argc, char **argv)
{
    int my_rank, num_procs, chunk_size, inicio, fin, palindromo = 1, result;
    char palabra[MAX_palabra_SIZE];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    //
    if (my_rank == 0)
    {
        // Proceso principal
        //printf("Ingrese una palabra: ");
        scanf("%s", palabra);//lee la cadena a verificar

        int palabra_len = strlen(palabra);
        chunk_size = palabra_len / num_procs;

        // Envía partes de la palabra a cada proceso
        for (int i = 1; i < num_procs; i++)
        {
            inicio = i * chunk_size;
            fin = inicio + chunk_size - 1;
            MPI_Send(&inicio, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&fin, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&palabra, MAX_palabra_SIZE, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        // Procesa su propia parte de la palabra
        inicio = 0;
        fin = chunk_size - 1;
        result = check_palindrome(palabra, inicio, fin);
    }
    else
    {
        // Proceso hijo
        MPI_Recv(&inicio, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&fin, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&palabra, MAX_palabra_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        result = check_palindrome(palabra, inicio, fin);
    }

    // Combina los resultados de cada proceso
    MPI_Reduce(&result, &palindromo, 1, MPI_INT, MPI_BAND, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        // Imprime el resultado
        if (palindromo)
        {
            printf("VERDAD\n");
        }
        else
        {
            printf("FALSO\n");
        }
    }

    MPI_Finalize();
    return 0;
}
