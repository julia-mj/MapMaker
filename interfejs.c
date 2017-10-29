#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <locale.h>
#include "mapa.h"
/*Julia Majkowska - Projekt końcowy
 * moduł główny implementujący fukncje związane z interfejsem*/
int zmiany_w_palecie = 0;
int wybrano = 0;
GtkImage* obrazek; 
GtkWidget* pudlo_z_podgladem;
GtkWidget* pudlo_na_poziomice;
GtkWidget* scroll;
GtkWidget* pudlo_na_przyciski;
GtkWidget* przyciskp;
GtkWidget* przyciskm;
GdkPixbuf* pixbuf;
paleta* aktualna_paleta;
mapa* aktualna_mapa;
double enlargement=1;

GtkWidget* domyslne_nowe_okno(char* tytul){
    GtkWidget * okienko = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(okienko),tytul);
    gtk_window_set_position(GTK_WINDOW(okienko),GTK_WIN_POS_CENTER_ALWAYS);
    gtk_container_set_border_width(GTK_CONTAINER(okienko), 10);
    return okienko;
}

static void destroy_pixbuf(GtkWidget* widget, GdkPixbuf* pixbuf){
    g_object_unref(pixbuf);
}

void resize_picture(){
    GdkPixbuf* nowy =  gdk_pixbuf_scale_simple(pixbuf, gdk_pixbuf_get_width(pixbuf)*enlargement, gdk_pixbuf_get_height(pixbuf)*enlargement, GDK_INTERP_BILINEAR);
    if(obrazek!= NULL) gtk_image_clear(obrazek);
    gtk_image_set_from_pixbuf(obrazek,  nowy);
    g_object_unref(nowy);
}

static void enlarge_picture(GtkWidget* widget, gpointer dane){
    enlargement*=2;
    resize_picture();
}
static void minimize_picture(GtkWidget* widget, gpointer dane){
    enlargement/=2;
    resize_picture();
}

static void add(GtkWidget* widget, gpointer dane){
    zmiany_w_palecie++;
}

static void zobacz(GtkWidget *widget,GtkWidget* pudelko){
    if(obrazek != NULL) gtk_image_clear(obrazek);
    if(scroll == NULL){
        scroll = gtk_scrolled_window_new(NULL, NULL); 
        gtk_scrolled_window_set_min_content_width   ((GtkScrolledWindow*)scroll,500);
        gtk_scrolled_window_set_min_content_height   ((GtkScrolledWindow*)scroll,500);
        gtk_box_pack_start(GTK_BOX(pudelko), scroll, TRUE, TRUE, 0); 
        gtk_container_add(GTK_CONTAINER(scroll), (GtkWidget*) obrazek);
    }
    if(pixbuf!= NULL){
        g_object_unref((gpointer)pixbuf);
        pixbuf = NULL;
    }
    pixbuf= zamien_na_kolorki(aktualna_paleta, aktualna_mapa);
    enlargement = ((double) 500/gdk_pixbuf_get_width(pixbuf));
    resize_picture();
    gtk_widget_show_all(pudelko);
}

guint reload_palette_view(int zmiany){
    if(zmiany_w_palecie==0) return 0;
    if((zmiany) !=zmiany_w_palecie) return 0;
    zmiany_w_palecie==0;
    gtk_container_forall ((GtkContainer *)pudlo_na_poziomice, zniszcz,NULL);
    poziom* p2 = aktualna_paleta->p;
    while(p2!=NULL){
     ustaw_poziomice(pudlo_na_poziomice, p2);
     p2 = p2->nastepny;        
    }  
    zobacz(pudlo_na_poziomice, pudlo_z_podgladem);
    return 0;
}

static void parsuj_wysokosc(GtkWidget * widget, poziom* teraz){
    const char* text = gtk_entry_get_text(GTK_ENTRY(widget));
    zmiany_w_palecie++;
    teraz->wysokosc = atoi(text);
    przenies(teraz, aktualna_paleta);
    int zmiany_teraz = zmiany_w_palecie;
    g_timeout_add_seconds(3, (GSourceFunc) reload_palette_view, zmiany_teraz);
}

static void zmien_kolor(GtkWidget * widget, poziom* teraz){
    zmiany_w_palecie++;
    GdkRGBA* kol = g_malloc(sizeof(GdkRGBA));
    gtk_color_chooser_get_rgba((GtkColorChooser*)widget,kol);
    teraz->kolor  =*kol;
    int zmiany_teraz = zmiany_w_palecie;
    g_timeout_add_seconds(5, (GSourceFunc) reload_palette_view, zmiany_teraz);
    g_free(kol);
}

static void usun_poziomice(GtkWidget * widget, poziom* teraz){
    usun_poziom(teraz, aktualna_paleta);
    gtk_widget_destroy(gtk_widget_get_parent( widget));    
}

static void save_pal(GtkWidget * widget, gpointer dane){
    GtkWidget* okienko = domyslne_nowe_okno("Zapisz palete");
    GtkWidget* plik = gtk_file_chooser_dialog_new("Zapisz palete", (GtkWindow*)okienko, GTK_FILE_CHOOSER_ACTION_SAVE, "ZAPISZ", GTK_RESPONSE_ACCEPT,"ANULUJ", GTK_RESPONSE_CANCEL, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation ((GtkFileChooser*) plik, TRUE);
    gtk_file_chooser_set_current_name ((GtkFileChooser*) plik, "Untitled document");
    gint res = gtk_dialog_run (GTK_DIALOG (plik));
    if (res == GTK_RESPONSE_ACCEPT){
        char *filename;

        filename = gtk_file_chooser_get_filename ((GtkFileChooser*) plik);
        zapisz_palete(aktualna_paleta, filename);
        g_free (filename);
    }
    gtk_widget_destroy (plik);
    gtk_widget_destroy (okienko);
}

static void save_map(GtkWidget * widget, GdkPixbuf* pixbuf){
    GtkWidget* okienko = domyslne_nowe_okno("Zapisz mape");
    GtkWidget* plik = gtk_file_chooser_dialog_new("Zapisz mape", (GtkWindow*) okienko, GTK_FILE_CHOOSER_ACTION_SAVE, "ZAPISZ", GTK_RESPONSE_ACCEPT,"ANULUJ", GTK_RESPONSE_CANCEL, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation ((GtkFileChooser*) plik, TRUE);
    gtk_file_chooser_set_current_name ((GtkFileChooser*) plik, "Untitled document");
    gint res = gtk_dialog_run (GTK_DIALOG (plik));
    if (res == GTK_RESPONSE_ACCEPT){
        char *filename;

        filename = gtk_file_chooser_get_filename ((GtkFileChooser*) plik);
        zapisz_mape_do_pliku(aktualna_mapa, aktualna_paleta, filename);
        g_free (filename);
    }
    gtk_widget_destroy (plik);
    gtk_widget_destroy (okienko);
}

static void read_pal(GtkWidget * widget, GtkWidget* pudlo_na_poziomice){
    GtkWidget* okienko = domyslne_nowe_okno("Wybor palety");
    GtkWidget* plik = gtk_file_chooser_dialog_new("Wybor palety", (GtkWindow*) okienko, GTK_FILE_CHOOSER_ACTION_OPEN, "WYBIERZ", GTK_RESPONSE_ACCEPT,"ANULUJ", GTK_RESPONSE_CANCEL, NULL);
    gint res = gtk_dialog_run (GTK_DIALOG (plik));
    if (res == GTK_RESPONSE_ACCEPT){
        
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (plik);
        filename = gtk_file_chooser_get_filename (chooser);
        aktualna_paleta = wczytaj_palete(filename, pudlo_na_poziomice, aktualna_paleta);
        zobacz(pudlo_na_poziomice, pudlo_z_podgladem);
        gtk_button_set_label((GtkButton*)widget, filename);
        g_free (filename);
    }
    gtk_widget_destroy (plik);
    gtk_widget_destroy (okienko);
}

void ustaw_poziomice( GtkWidget* pudelko, poziom* teraz){
    GdkRGBA kolor = teraz->kolor;
    GtkWidget* nowy_wiersz = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    GtkWidget* wartosc = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(wartosc), 200);
    char* tekst_startowy = malloc(50*sizeof(int));
    sprintf(tekst_startowy, "%d", teraz->wysokosc);
    gtk_entry_set_text ((GtkEntry*) wartosc, tekst_startowy);
    g_signal_connect(G_OBJECT(wartosc), "changed",G_CALLBACK(parsuj_wysokosc),(gpointer) teraz);
    gtk_box_pack_start(GTK_BOX(nowy_wiersz), wartosc, TRUE, TRUE, 0);
    GtkWidget* kolorek = gtk_color_button_new_with_rgba(&kolor);
    gtk_box_pack_start(GTK_BOX(nowy_wiersz), kolorek, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(kolorek), "color-set", G_CALLBACK(zmien_kolor), (gpointer) teraz);
    g_signal_connect(G_OBJECT(kolorek), "clicked", G_CALLBACK(add), (gpointer) teraz);
    GtkWidget* usun = gtk_button_new_with_label("USUN");
    gtk_box_pack_start(GTK_BOX(nowy_wiersz), usun, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(usun), "clicked", G_CALLBACK(usun_poziomice), (gpointer) teraz);
    gtk_box_pack_start(GTK_BOX(pudelko), nowy_wiersz, TRUE, TRUE, 0);
    gtk_widget_show_all(pudelko);
}


static void dodaj_poziomice(GtkWidget *widget,GtkWidget *pudelko){
    GdkRGBA* kolor = g_malloc(sizeof(GdkRGBA));
    gdk_rgba_parse (kolor,"YellowGreen");
    poziom* teraz = nowy_poziom(aktualna_paleta, 0, *kolor);
    GtkWidget* nowy_wiersz = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    GtkWidget* wartosc = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(wartosc), 200);
    gtk_entry_set_placeholder_text ((GtkEntry*) wartosc, "Wprowadz wartosc poziomicy");
    g_signal_connect(G_OBJECT(wartosc), "changed",G_CALLBACK(parsuj_wysokosc),(gpointer) teraz);
    gtk_box_pack_start(GTK_BOX(nowy_wiersz), wartosc, TRUE, TRUE, 0);
    GtkWidget* kolorek = gtk_color_button_new_with_rgba(kolor);
    g_free(kolor);
    gtk_box_pack_start(GTK_BOX(nowy_wiersz), kolorek, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(kolorek), "color-set", G_CALLBACK(zmien_kolor), (gpointer) teraz);
    g_signal_connect(G_OBJECT(kolorek), "clicked", G_CALLBACK(add), (gpointer) teraz);
    GtkWidget* usun = gtk_button_new_with_label("USUN");
    gtk_box_pack_start(GTK_BOX(nowy_wiersz), usun, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(usun), "clicked", G_CALLBACK(usun_poziomice), (gpointer) teraz);
    gtk_box_pack_start(GTK_BOX(pudelko), nowy_wiersz, TRUE, TRUE, 0);
    gtk_widget_show_all(pudelko);
}


static void clear_palette(GtkWidget* widget, gpointer dane){
    wyczysc_palete(aktualna_paleta);    
    gtk_container_forall ((GtkContainer *)dane, zniszcz,NULL);
}
static void default_palette(GtkWidget* widget, gpointer dane){
    paleta_domyslna(aktualna_mapa->minval, aktualna_mapa->maxval, dane, aktualna_paleta);
    zobacz(pudlo_na_poziomice, pudlo_z_podgladem);
}

void make_picture_size_buttons(){
    pudlo_na_przyciski = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    przyciskp = gtk_button_new_with_label("+");
    przyciskm = gtk_button_new_with_label("-");
    gtk_box_pack_start(GTK_BOX(pudlo_na_przyciski), przyciskp, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(pudlo_na_przyciski), przyciskm, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(pudlo_z_podgladem), pudlo_na_przyciski, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(przyciskp), "clicked", G_CALLBACK(enlarge_picture), NULL);
    g_signal_connect(G_OBJECT(przyciskm), "clicked", G_CALLBACK(minimize_picture), NULL);
    gtk_widget_show_all(pudlo_z_podgladem);
}

void make_palette_buttons(GtkWidget* przyciski){
    GtkWidget* plik = gtk_button_new_with_label("WCZYTAJ PALETĘ");
    g_signal_connect(G_OBJECT(plik),"clicked", G_CALLBACK(read_pal), (gpointer)pudlo_na_poziomice);
    gtk_box_pack_start(GTK_BOX(przyciski), plik, TRUE, TRUE, 0);
    
    GtkWidget * zapisz_pal  = gtk_button_new_with_label("ZAPISZ PALETĘ");
    g_signal_connect(G_OBJECT(zapisz_pal), "clicked", G_CALLBACK(save_pal), NULL);
    gtk_box_pack_start(GTK_BOX(przyciski), zapisz_pal, TRUE, TRUE, 0);
    
    GtkWidget * domyslna_paleta = gtk_button_new_with_label("ZASTOSUJ DOMYSLNA PALETĘ");
    g_signal_connect(G_OBJECT(domyslna_paleta),"clicked", G_CALLBACK(default_palette),pudlo_na_poziomice);
    gtk_box_pack_start(GTK_BOX(przyciski), domyslna_paleta, TRUE, TRUE, 0);
    
    
}

static void okno_wyboru_poziomic(GtkWidget *widget, gpointer dane){
    obrazek = ((GtkImage *)gtk_image_new()) ;
    GtkWidget* pudlo = gtk_widget_get_parent(widget);
    pudlo_z_podgladem = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(pudlo), pudlo_z_podgladem, TRUE, TRUE, 0);
    
    GtkWidget *pudelko =gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    pudlo_na_poziomice = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(pudlo_z_podgladem), pudelko, TRUE, TRUE, 0);
    
    GtkWidget * przyciski = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(pudelko), przyciski, TRUE, TRUE, 0);
    
    make_palette_buttons(przyciski);
    
    GtkWidget * podglad = gtk_button_new_with_label("ODŚWIEŻ PODGLAD");
    g_signal_connect(G_OBJECT(podglad),"clicked", G_CALLBACK(zobacz), pudlo_z_podgladem);
    //g_timeout_add_seconds(60, (GSourceFunc) zobacz, pudlo_z_podgladem);
    gtk_box_pack_start(GTK_BOX(przyciski), podglad, TRUE, TRUE, 0);
   
    GtkWidget * zapisz  = gtk_button_new_with_label("ZAPISZ MAPĘ");
    g_signal_connect(G_OBJECT(zapisz), "clicked", G_CALLBACK(save_map), NULL);
    gtk_box_pack_start(GTK_BOX(przyciski), zapisz, TRUE, TRUE, 0);
    
    GtkWidget *nowa_poziomica= gtk_button_new_with_label("NOWA POZIOMICA");
    g_signal_connect(G_OBJECT(nowa_poziomica), "clicked", G_CALLBACK(dodaj_poziomice), (gpointer) pudlo_na_poziomice);
    gtk_box_pack_start(GTK_BOX(pudelko), nowa_poziomica, TRUE, TRUE, 0);
    
    GtkWidget *wyczysc_poziomice= gtk_button_new_with_label("WYCZYSC POZIOMICE");
    g_signal_connect(G_OBJECT(wyczysc_poziomice), "clicked", G_CALLBACK(clear_palette), (gpointer) pudlo_na_poziomice);
    gtk_box_pack_start(GTK_BOX(pudelko), wyczysc_poziomice, TRUE, TRUE, 0);
    
    gtk_box_pack_start(GTK_BOX(pudelko), pudlo_na_poziomice, TRUE, TRUE, 0);
    zobacz(pudlo, pudlo_z_podgladem);
    make_picture_size_buttons();
    gtk_widget_show_all(pudlo);   
}

static void wybor_pliku_wejsciowego(GtkWidget* widget, gpointer dane){
    GtkWidget* okienko = domyslne_nowe_okno("Wybor pliku wejsciowego");
    GtkWidget* plik = gtk_file_chooser_dialog_new("Wybor pliku wejsciowego", (GtkWindow*) okienko, GTK_FILE_CHOOSER_ACTION_OPEN, "WYBIERZ", GTK_RESPONSE_ACCEPT,"ANULUJ", GTK_RESPONSE_CANCEL, NULL);
    gint res = gtk_dialog_run (GTK_DIALOG (plik));
    if (res == GTK_RESPONSE_ACCEPT){
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (plik);
        filename = gtk_file_chooser_get_filename (chooser);
        zniszcz_mape(aktualna_mapa);
        aktualna_mapa = wczytaj_mape_z_formatu(filename);
        if(wybrano==0){
            okno_wyboru_poziomic(widget, dane);
            wybrano=1;
        }
        else zobacz(widget, pudlo_z_podgladem);
        gtk_button_set_label((GtkButton*)widget, filename);
        g_free (filename);
    }
    gtk_widget_destroy (plik);
    gtk_widget_destroy (okienko);
}

static void pierwsze_okno(){
    GtkWidget *window = domyslne_nowe_okno("Generator mapy");
    g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size((GtkWindow*)window, 300, 300);
    GtkWidget *pudelko =gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), pudelko);
    GtkWidget *przycisk = gtk_button_new_with_label("WYBIERZ PLIK WEJSCIOWY");
    gtk_box_pack_start(GTK_BOX(pudelko), przycisk, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(przycisk), "clicked", G_CALLBACK(wybor_pliku_wejsciowego), pudelko);
    gtk_widget_show_all(window);
}

int main( int argc,char *argv[] ){
    gtk_init(&argc, &argv);
    setlocale(LC_ALL, "C");
    aktualna_paleta= nowa_paleta();
    pierwsze_okno();
    gtk_main();
    return 0;
}
