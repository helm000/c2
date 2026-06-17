#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define ROZMIAR 4096

int main(void) {
    //stwarzamy semafory
    sem_t* sem_gotowe = sem_open("sem_gotowe", O_CREAT, 0666, 0);
    sem_t* sem_przetworzone = sem_open("sem_przetworzone", O_CREAT, 0666, 0);

    //czekamy aż będzie gotowe
    sem_wait(sem_gotowe);

    //tworzymy pamiec i wskaznik do niej
    int pam = shm_open("pamiec_wspolna", O_RDWR | O_CREAT, 0666);
    ftruncate(pam, ROZMIAR);
    void *wsk = mmap(NULL , ROZMIAR, PROT_READ | PROT_WRITE, MAP_SHARED, pam, 0);

    //mapujemy na char i zmieniamy na lower
    char* tekst = (char*) wsk;
    for (int i = 0; i < ROZMIAR-1; i++) {
        tekst[i] = tolower((unsigned char)tekst[i]);
    }

    //wywalamy wskaznik w cholere i dajemy zielone światło post
    munmap(wsk, ROZMIAR);
    sem_post(sem_przetworzone);

    //czyszczenie
    sem_close(sem_gotowe);
    sem_close(sem_przetworzone);
    sem_unlink("sem_gotowe");
    sem_unlink("sem_przetworzone");
    return 0;
}