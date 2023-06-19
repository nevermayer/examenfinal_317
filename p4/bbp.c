#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <mpfr.h>

#define PRECISION 1000

typedef struct {
    int size;
    unsigned char *data;
} MPI_MPFR;

void MPI_MPFR_create(MPI_MPFR *mpfr) {
    mpfr_init2(mpfr, PRECISION);
    mpfr_set_ui(*mpfr, 0, MPFR_RNDN);
}

void MPI_MPFR_destroy(MPI_MPFR *mpfr) {
    mpfr_clear(*mpfr);
}

void MPI_MPFR_send(MPI_MPFR *mpfr, int dest, int tag) {
    int count;
    unsigned char *buffer;

    mpfr_exp_t exp;
    mpfr_prec_t prec = mpfr_get_prec(*mpfr);
    count = mpfr_export(NULL, &exp, 1, sizeof(unsigned char), 0, 0, *mpfr, MPFR_RNDN);
    buffer = (unsigned char*)malloc(count * sizeof(unsigned char));
    mpfr_export(buffer, &exp, 1, sizeof(unsigned char), 0, 0, *mpfr, MPFR_RNDN);
    MPI_Send(&prec, sizeof(mpfr_prec_t), MPI_BYTE, dest, tag, MPI_COMM_WORLD);
    MPI_Send(&exp, sizeof(mpfr_exp_t), MPI_BYTE, dest, tag, MPI_COMM_WORLD);
    MPI_Send(buffer, count, MPI_BYTE, dest, tag, MPI_COMM_WORLD);
    free(buffer);
}

void MPI_MPFR_recv(MPI_MPFR *mpfr, int source, int tag) {
    int count;
    unsigned char *buffer;

    mpfr_exp_t exp;
    mpfr_prec_t prec;
    MPI_Status status;
    MPI_Recv(&prec, sizeof(mpfr_prec_t), MPI_BYTE, source, tag, MPI_COMM_WORLD, &status);
    MPI_Recv(&exp, sizeof(mpfr_exp_t), MPI_BYTE, source, tag, MPI_COMM_WORLD, &status);
    mpfr_init2(*mpfr, prec);
    count = mpfr_import(*mpfr, 1, sizeof(unsigned char), sizeof(unsigned char), 0, 0, &exp, status.count, status.MPI_SOURCE, MPFR_RNDN);
    buffer = (unsigned char*)malloc(count * sizeof(unsigned char));
    MPI_Recv(buffer, count, MPI_BYTE, source, tag, MPI_COMM_WORLD, &status);
    mpfr_import(*mpfr, 1, sizeof(unsigned char), sizeof(unsigned char), 0, 0, &exp, count, buffer, MPFR_RNDN);
    free(buffer);
}

int main(int argc, char *argv[]) {
    int rank, size, num_digits, chunk_size, start, end;
    double start_time, end_time;
    MPI_MPFR pi, term;

    mpfr_set_default_prec(PRECISION);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    num_digits = 1000;
    chunk_size = num_digits / size;
    start = rank * chunk_size;
    end = start + chunk_size - 1;

    MPI_MPFR_create(&pi);
    MPI_MPFR_create(&term);

    // Realiza los cálculos con los números MPFR

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    MPI_MPFR_send(&pi, 0, 0);

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    if (rank == 0) {
        MPI_MPFR_recv(&pi, 0, 0);
        printf("Pi: ");
        mpfr_out_str(stdout, 10, num_digits, pi, MPFR_RNDN);
        printf("\n");

        printf("Tiempo total: %.6f segundos\n", end_time - start_time);
    }

    MPI_MPFR_destroy(&pi);
    MPI_MPFR_destroy(&term);

    MPI_Finalize();

    return 0;
}
