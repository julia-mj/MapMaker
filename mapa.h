#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <locale.h>
#include "paletalib.h"
#include "gdal.h"

/*Julia Majkowska - Projekt końcowy
 * plik z deklaracjami funkcji zdefininiowanych w module obrazek.c */

typedef struct{
    int h, w, maxval, minval;
    int ** obraz;
        
}mapa;

void zniszcz_mape(mapa* m);
mapa* stworz_nowa_mape(int h, int w);
mapa* wczytaj_mape_z_formatu(char* nazwa);
void zapisz_mape_do_pliku(mapa* m, paleta* pal, const char* nazwa);
GdkRGBA* znajdz_kolor(int wysokosc, paleta* pal);
short get_green(GdkRGBA* kolor);
short get_red(GdkRGBA* kolor);
short get_blue(GdkRGBA* kolor);
void zmien_pixel( GdkPixbuf* imageptr, int x, int y, GdkRGBA* kol); //http://stackoverflow.com/questions/16785886/get-pixel-value-on-gdkpixbuf-set-pixel-value-with-gdkcairo

GdkPixbuf* zamien_na_kolorki(paleta* pal, mapa* m);
