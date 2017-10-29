#include<stdio.h>
#include<stdlib.h>
#include <gtk/gtk.h>
/*Julia Majkowska - Projekt końcowy
 * plik z deklaracjami funkcji z pliku paleta.c oraz funkcji ustaw poziomice z pliku interfejs*/

struct poziom{
    int wysokosc;
    GdkRGBA kolor;
    struct poziom* nastepny;
    struct poziom* poprzedni;
};
typedef struct poziom poziom;
typedef struct {// posortowana lista wartosci poziomic z przypisamymi supremami i kolorami
    poziom* p; 
    poziom* k;
} paleta;

void zniszcz(GtkWidget* widget, gpointer dane);
paleta* nowa_paleta ();
void wyczysc_palete(paleta *pal);
void usun_palete(paleta *pal);
paleta* paleta_domyslna(int min, int max, GtkWidget *pudelko, paleta* pal);
poziom* przenies(poziom* nowy, paleta* pal);
poziom* nowy_poziom( paleta *pal,  int w, GdkRGBA kol);
void usun_poziom(poziom * p, paleta * pal);
void zapisz_palete( paleta* p, char* nazwa_pliku);
void ustaw_poziomice( GtkWidget* pudelko, poziom* teraz);
paleta* wczytaj_palete(char* nazwa_pliku, GtkWidget* pudelko, paleta* p);
