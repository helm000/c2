#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define ROZMIAR 4096

int main(void) {
    //otwierasz plik i tworzysz tablice na dane w nim
    FILE* plik = fopen("plik.txt", "r");
    char dane[ROZMIAR];
    memset(dane, 0, ROZMIAR);

    //wkladasz plik do tablicy
    fread(dane, 1, ROZMIAR, plik);
    fclose(plik);

    //tworzysz pamiec wspolna
    int pam = shm_open("pamiec_wspolna", O_RDWR | O_CREAT, 0666);
    ftruncate(pam, ROZMIAR);

    //wskaznik na ta pamiec i wkladasz tam tablice
    void* wsk = mmap(NULL, ROZMIAR, PROT_READ | PROT_WRITE, MAP_SHARED, pam, 0);
    memcpy(wsk, dane, ROZMIAR);

    //utworzenie semaforow
    sem_t* sem_gotowe = sem_open("sem_gotowe", O_CREAT, 0644, 0);
    sem_t* sem_przetworzone = sem_open("sem_przetworzone", O_CREAT, 0644, 0);

    //informacja ze wstawiles swoje i czekasz na przetworzone
    sem_post(sem_gotowe);
    sem_wait(sem_przetworzone);

    //otwierasz plik do zapisu i wpieprzasz tam tablice z shared_memory
    FILE* wynik = fopen("wynik.txt", "w");
    fprintf(wynik, "%s\n", (char*)wsk);
    fclose(wynik);

    //zamykanie wszystkiego
    munmap(wsk, ROZMIAR);
    sem_close(sem_gotowe);
    sem_close(sem_przetworzone);
    sem_unlink("sem_gotowe");
    sem_unlink("sem_przetworzone");
    return 0;
}
