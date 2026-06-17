## Instrukcja Kompilacji i Uruchomienia


2
### 1. Kompilacja
Skompiluj oba programy, używając flag `-lrt` (dla pamięci współdzielonej) oraz `-lpthread` (dla semaforów):

```bash
gcc pamiecZapis2.c -o zapis -lrt -lpthread
gcc pamiecOdczyt2.c -o odczyt -lrt -lpthread
