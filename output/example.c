#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration generated from the global KAMUS
bool IsLulus(int N);

// KAMUS: Type Definitions
typedef struct
{
    char *Nama;
    int Nilai;
} Mahasiswa;

// The main ALGORITMA block becomes the main function
int main()
{
    // KAMUS: Variable Declarations
    Mahasiswa Mhs1;
    int i;

    // ALGORITMA: The main logic starts here
    for (i = 1; i <= 3; i++)
    {
        printf("Masukkan Nama Mahasiswa ke-%d: \n", i);

        Mhs1.Nama = (char *)malloc(100 * sizeof(char));
        if (Mhs1.Nama == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        scanf("%99s", Mhs1.Nama);

        printf("Masukkan Nilai %s: \n", Mhs1.Nama);
        scanf("%d", &Mhs1.Nilai);

        // Function call inside the if condition
        if (IsLulus(Mhs1.Nilai))
        {
            printf("%s dinyatakan LULUS.\n", Mhs1.Nama);
        }
        else
        {
            printf("%s dinyatakan GAGAL.\n", Mhs1.Nama);
        }

        printf("\n");

        free(Mhs1.Nama);
        Mhs1.Nama = NULL;
    }

    printf("Proses selesai.\n");

    return 0;
}

// Function Implementation generated from the implementation block
bool IsLulus(int N)
{
    // KAMUS LOKAL
    bool hasil;

    // ALGORITMA
    if (N >= 65)
    {
        hasil = true;
    }
    else
    {
        hasil = false;
    }
    // '-> hasil' is translated to a return statement
    return hasil;
}