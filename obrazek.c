#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <locale.h>
#include "gdal.h"
//#include "cpl_conv.h"
#include "mapa.h"

/*Julia Majkowska - Projekt końcowy
 * moduł implementujący fukncje związane z operacjami na mapie*/

/*typedef struct{
    int h, w, maxval, minva;;
    int ** obraz;
        
}mapa;*/
void zniszcz_mape(mapa* m){
    if(m==NULL) return;
    for(int i = 0; i<(m->h); i++) free(m->obraz[i]);
    free(m->obraz);
    free(m);
}

mapa* stworz_nowa_mape(int h, int w){
    mapa* m = malloc(sizeof(mapa));
    m->h = h;
    m->w = w;
    m->maxval =0; 
    m->obraz =(int**) malloc(sizeof(int*)*h);
    for(int i = 0; i<h; i++) m->obraz[i] = malloc(sizeof(int)*w); 
    return m;
}



mapa* wczytaj_mape_z_formatu(char* nazwa){// kod zaczerpniety po czesci z http://www.gdal.org/gdal_tutorial.html
    GDALDatasetH  hDataset;
    GDALAllRegister();
    if(nazwa == NULL){
        return NULL;
    }
    hDataset = GDALOpen( nazwa, GA_ReadOnly );
    if( hDataset == NULL )
    {
       printf("NIE MA TAKIEGO PLIKU\n");
       return NULL;
    }
    GDALRasterBandH hBand;
    hBand = GDALGetRasterBand( hDataset, 1 );
    
    int nBlockXSize, nBlockYSize;
    GDALGetBlockSize( hBand, &nBlockXSize, &nBlockYSize );
    int   nXSize = GDALGetRasterBandXSize( hBand );
    int   nYSize = GDALGetRasterBandYSize( hBand ); 
    mapa* wynik = stworz_nowa_mape(nYSize, nXSize);
    wynik->minval = (1<<15);
    wynik->maxval = 0;
    if(nBlockXSize!=1 && nBlockYSize!=1){
        printf("Nieobslugiwany format pliku, wybierz inny plik\n");
        return wynik;
    }
    else if(nBlockXSize ==1){
        short * bufor = malloc(nBlockYSize*sizeof(int));
        for(int i = 0; i<nXSize; i++){
            GDALRasterIO( hBand, GF_Read, i,0, nBlockXSize, nBlockYSize,
              bufor, nBlockXSize, nBlockYSize, GDT_Int16,
              0, 0 );
            for(int j = 0; j<nBlockYSize; j++){
                wynik->obraz[i][j] =  bufor[j];
            }
        }
        free(bufor);
    }
    else if(nBlockYSize ==1){
        short * bufor = malloc(nBlockXSize*sizeof(int));
        for(int i = 0; i<nYSize; i++){
            GDALRasterIO( hBand, GF_Read, 0, i, nBlockXSize, nBlockYSize,
              bufor, nBlockXSize, nBlockYSize, GDT_Int16,
              0, 0 );
            for(int j = 0; j<nBlockXSize; j++){
                wynik->obraz[j][i] = bufor[j];
            }
        }
       free(bufor);
    }
   int             bGotMin, bGotMax;
    double          adfMinMax[2];
    adfMinMax[0] = GDALGetRasterMinimum( hBand, &bGotMin );
    adfMinMax[1] = GDALGetRasterMaximum( hBand, &bGotMax );
    if( ! (bGotMin && bGotMax) )
        GDALComputeRasterMinMax( hBand, TRUE, adfMinMax );
    wynik->minval = adfMinMax[0];
    wynik->maxval = adfMinMax[1];
    GDALDereferenceDataset(hDataset);
    GDALClose(hDataset);
    return wynik;
    
}

void zapisz_mape_do_pliku(mapa* m, paleta* pal, const char* nazwa){
    
    GdkPixbuf* pixbuf = zamien_na_kolorki(pal, m);
    gdk_pixbuf_save (pixbuf, nazwa, "png",NULL,NULL);
}

GdkRGBA* znajdz_kolor(int wysokosc, paleta* pal){
    poziom* p = pal->p;
    while(p!= NULL && (p->wysokosc < wysokosc)){// poziomica jest supremum obejmowanych punktow
        p= p->nastepny;        
    }
    GdkRGBA* kolor; 
    if(p==NULL) return NULL;
    else kolor =  &(p->kolor);
    return kolor;
}


short get_green(GdkRGBA* kolor){
    return 0.5+kolor->green*255.;
}
short get_red(GdkRGBA* kolor){
    return 0.5+kolor->red*255.;
}
short get_blue(GdkRGBA* kolor){
    return 0.5+kolor->blue*255.;
}

void zmien_pixel( GdkPixbuf* imageptr, int x, int y, GdkRGBA* kol) //zrodlo tej funkcji http://stackoverflow.com/questions/16785886/get-pixel-value-on-gdkpixbuf-set-pixel-value-with-gdkcairo
   {
   if ( kol == NULL) return;
   if ( imageptr==NULL ) return;
   if ( !( x>=0 && y>=0 && x<gdk_pixbuf_get_width(imageptr) && y<gdk_pixbuf_get_height(imageptr) ) ) return;

   int offset = y*gdk_pixbuf_get_rowstride(imageptr) + x*gdk_pixbuf_get_n_channels(imageptr);
   guchar * pixel = &gdk_pixbuf_get_pixels(imageptr)[ offset ]; // get pixel pointer
   pixel[0] = get_red(kol);
   pixel[1] = get_green(kol);
   pixel[2] = get_blue(kol);
}

GdkPixbuf* zamien_na_kolorki(paleta* pal, mapa* m){
    GdkPixbuf* wynik = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, m->w, m->h);
    for(int i = 0; i<m->h; i++)
        for(int j = 0; j<m->w; j++){
                GdkRGBA* aktkolor = znajdz_kolor((m->obraz)[i][j], pal);
                if(aktkolor==NULL){
                    aktkolor = malloc( sizeof(GdkRGBA));
                    gdk_rgba_parse(aktkolor, "Black");
                    zmien_pixel(wynik, i, j, aktkolor);
                    g_free(aktkolor);
                }
                else zmien_pixel(wynik, i, j, aktkolor);
        }
    return wynik;
}