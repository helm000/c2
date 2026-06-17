#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define ROZMIAR_KOMUNIKATU 4096   // maksymalny rozmiar tekstu w jednym komunikacie
#define ROZMIAR_PORCJI 800        // ile bajtow na raz czytamy z pliku (margines na rozszerzenie cyfr w slowa)

char *slowa[10] = {"zero","one","two","three","four","five","six","seven","eight","nine"};

struct komunikat {
    long typ;                       // 1 = rodzic -> potomek, 2 = potomek -> rodzic
    char tekst[ROZMIAR_KOMUNIKATU];
};

int main() {
    int kolejka = msgget((key_t)1234, 0666 | IPC_CREAT);

    pid_t pid = fork();

    if (pid == 0) {
        // PROCES 2 - odbiera porcje tekstu z kolejki, zamienia cyfry na slowa, odsyla wynik
        struct komunikat odebrany, doWyslania;
        int dlugosc;

        while ((dlugosc = msgrcv(kolejka, &odebrany, ROZMIAR_KOMUNIKATU, 1, 0)) > 0) {
            char wynik[ROZMIAR_KOMUNIKATU];
            int pozycja = 0;

            for (int i = 0; i < dlugosc; i++) {
                char znak = odebrany.tekst[i];
                if (znak >= '0' && znak <= '9') {
                    char *slowo = slowa[znak - '0'];
                    strcpy(wynik + pozycja, slowo);
                    pozycja += strlen(slowo);
                } else {
                    wynik[pozycja++] = znak;
                }
            }

            doWyslania.typ = 2;
            memcpy(doWyslania.tekst, wynik, pozycja);
            msgsnd(kolejka, &doWyslania, pozycja, 0);
        }

        // pusty komunikat = sygnal konca danych dla rodzica
        doWyslania.typ = 2;
        msgsnd(kolejka, &doWyslania, 0, 0);
        exit(0);

    } else {
        // PROCES 1 - czyta plik, wysyla porcje do potomka, odbiera przetworzony tekst, zapisuje plik
        struct komunikat doWyslania, odebrany;
        doWyslania.typ = 1;

        FILE *wejscie = fopen("plik.txt", "r");
        if (wejscie == NULL) {
            perror("Nie mozna otworzyc plik.txt");
            exit(1);
        }

        char bufor[ROZMIAR_PORCJI];
        int n;
        while ((n = fread(bufor, 1, ROZMIAR_PORCJI, wejscie)) > 0) {
            memcpy(doWyslania.tekst, bufor, n);
            msgsnd(kolejka, &doWyslania, n, 0);
        }
        fclose(wejscie);

        // pusty komunikat = sygnal konca danych dla potomka
        msgsnd(kolejka, &doWyslania, 0, 0);

        FILE *wyjscie = fopen("wynik.txt", "w");
        int dlugosc;
        while ((dlugosc = msgrcv(kolejka, &odebrany, ROZMIAR_KOMUNIKATU, 2, 0)) > 0) {
            fwrite(odebrany.tekst, 1, dlugosc, wyjscie);
        }
        fclose(wyjscie);

        wait(NULL);
        msgctl(kolejka, IPC_RMID, NULL); // usuniecie kolejki po zakonczeniu
    }

    return 0;
}
