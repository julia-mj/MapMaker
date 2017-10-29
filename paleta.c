#include<stdio.h>
#include<stdlib.h>
#include <gtk/gtk.h>
#include "paletalib.h"

/*Julia Majkowska - Projekt końcowy
 * moduł implementujący fukncje związane z modyfikacją palety*/
/*struct poziom{
    int wysokosc;
    GdkRGBA kolor;
    struct poziom* nastepny;
    struct poziom* poprzedni;
};
typedef struct poziom poziom;
typedef struct {// posortowany wektor wartosci poziomic z przypisamymi supremami i kolorami
    poziom* p; 
    poziom* k;
} paleta;
*/
void zniszcz(GtkWidget* widget, gpointer dane){
    gtk_widget_destroy(widget);
}

paleta* nowa_paleta (){
     paleta* wynik = malloc(sizeof(paleta));
     wynik->p = NULL;
     wynik->k = NULL;
     return wynik;
}

void wyczysc_palete(paleta *pal){
    
    poziom* p2 = pal->p;
    while(p2!=NULL){
     poziom* p3 = p2->nastepny;
     usun_poziom(p2, pal);
     p2 = p3;        
    }
    pal->p=NULL;
    pal->k=NULL;
}

void usun_palete(paleta*pal){
    wyczysc_palete(pal);
    free(pal->p);
    free(pal->k);
    free(pal);    
}

paleta* paleta_domyslna(int min, int max, GtkWidget* pudelko, paleta* pal){
    GdkRGBA tab[]={{0.018400, 0.306667, 0.086700, 1.000000},
                    {0.305882, 0.603922, 0.023529, 1.000000},
                    {0.603922, 0.803922, 0.196078, 1.000000},
                    {0.988235, 0.913725, 0.309804, 1.000000},
                    {0.768627, 0.627451, 0.000000, 1.000000},
                    {0.960784, 0.474510, 0.000000, 1.000000},
                    {0.937255, 0.160784, 0.160784, 1.000000},
                    {0.750000, 0.250000, 0.250000, 1.000000},
                    {0.643137, 0.000000, 0.000000, 1.000000},
                    {0.290000, 0.169402, 0.066700, 1.000000},
                    {0.463333, 0.369870, 0.302711, 1.000000},
                    {0.533333, 0.541176, 0.521569, 1.000000},
                    {0.827451, 0.843137, 0.811765, 1.000000},
                    {1.000000, 1.000000, 1.000000, 1.000000}};
    if(pal==NULL) pal = nowa_paleta();
    wyczysc_palete(pal);
    gtk_container_forall ((GtkContainer *)pudelko, zniszcz,NULL);
    for(int i = 1; i<=14; i++){
        int wys = min+((max-min+14)*i)/14;
        poziom* p = nowy_poziom(pal, wys, tab[i-1]); 
        ustaw_poziomice(pudelko, p);
    }    
    return pal;
}

poziom* przenies(poziom* nowy, paleta* pal){
    if(nowy->poprzedni!=NULL) nowy->poprzedni->nastepny = nowy->nastepny;
    if(nowy->nastepny!=NULL) nowy->nastepny->poprzedni = nowy->poprzedni;
    if(nowy->nastepny == NULL){
        pal->k = nowy->poprzedni;
    }
    if(nowy->poprzedni == NULL){
        pal->p = nowy->nastepny;
    }
    poziom* p2 = pal->p;
    while(p2!=NULL && ((p2->wysokosc)<nowy->wysokosc)){
        p2 = p2->nastepny;        
    }
    nowy->nastepny = p2;
    if(p2!=NULL){
        if(p2->poprzedni!= NULL) p2->poprzedni->nastepny =nowy;
        else{
            pal->p = nowy;
            nowy->poprzedni = NULL;
        }
        nowy->poprzedni = p2->poprzedni;
        p2->poprzedni = nowy;
    }
    else{
        nowy->poprzedni = pal->k;
        if(pal->k!=NULL) pal->k->nastepny = nowy;
        pal->k = nowy;
        nowy->nastepny = NULL;
    }
        
    if(pal->p == NULL){
        pal->p = nowy;
    }
    
    
}

poziom* nowy_poziom( paleta *pal,  int w, GdkRGBA kol){
    poziom* nowy = malloc(sizeof(poziom));
    nowy->wysokosc = w;
    nowy->kolor = kol;
    poziom* p2 = pal->p;
    nowy->poprzedni = NULL;
    nowy->nastepny = NULL;
    while(p2!=NULL && ((p2->wysokosc)<w)){
        p2 = p2->nastepny;        
    }
    nowy->nastepny = p2;
    if(p2!=NULL){
        if(p2->poprzedni!= NULL) p2->poprzedni->nastepny =nowy;
        else{
            pal->p = nowy;
            nowy->poprzedni = NULL;
        }
        nowy->poprzedni = p2->poprzedni;
        p2->poprzedni = nowy;
    }
    else{
        nowy->poprzedni = pal->k;
        if(pal->k!=NULL) pal->k->nastepny = nowy;
        pal->k = nowy;
        nowy->nastepny = NULL;
    }
        
    if(pal->p == NULL){
        pal->p = nowy;
    }
    return nowy;
}
void usun_poziom(poziom * p, paleta * pal){
    if(p->poprzedni!=NULL) p->poprzedni->nastepny = p->nastepny;
    if(p->nastepny!=NULL) p->nastepny->poprzedni = p->poprzedni;
    if(p->nastepny == NULL){
        pal->k = p->poprzedni;
    }
    if(p->poprzedni == NULL){
        pal->p = p->nastepny;
    }
    free(p);    
}

void zapisz_palete( paleta* p, char* nazwa_pliku){
    FILE * plik = fopen(nazwa_pliku, "w");
    poziom * poczatek = p->p;
    while(poczatek!=NULL){
        fprintf(plik, "%d ", poczatek->wysokosc);
        GdkRGBA kol = poczatek->kolor;
        fprintf(plik, "%lf %lf %lf %lf\n", kol.red, kol.green, kol.blue, kol.alpha);
        poczatek = poczatek->nastepny;
    }
    fclose(plik);
}

paleta* wczytaj_palete(char* nazwa_pliku, GtkWidget* pudelko, paleta* pal){
    if(pal==NULL) pal = nowa_paleta();
    wyczysc_palete(pal);    
    gtk_container_forall ((GtkContainer *)pudelko, zniszcz,NULL);
    FILE * plik = fopen(nazwa_pliku, "r");

    while(!feof(plik)){
        int wysokosc;
        fscanf(plik, "%d",&wysokosc);
        if(feof(plik)) return pal;
        GdkRGBA kol;
        fscanf(plik, "%lf %lf %lf %lf\n", &kol.red, &kol.green, &kol.blue, &kol.alpha);
        poziom* p = nowy_poziom(pal, wysokosc, kol);
        ustaw_poziomice(pudelko, p);
    };
    fclose(plik);
    return pal;
}