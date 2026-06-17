2
gcc pamiecZapis2.c -o zapis -lrt -lpthread
gcc pamiecOdczyt2.c -o odczyt -lrt -lpthread

echo "To Jest PRZYKladowy TEKST z Wielkimi Literami" > plik.txt

# w jednym terminalu
./zapis
# w drugim terminalu (mozna nawet wczesniej)
./odczyt

cat wynik.txt
