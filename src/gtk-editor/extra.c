#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dictionary.h"
#include "editor.h"
#include "word_list.h"

Dictionary* current_dict = NULL;
char* lang_buffer = NULL;
int* lang_position = NULL;
int lang_count = 0;
size_t lang_list_len = -1;
bool dictionary_changed = false;
int current_dict_pos = -1;

static void info_msg (const gchar *msg) {
    GtkWidget *dialog;  // pop up window with only OK button

    dialog = gtk_message_dialog_new(GTK_WINDOW(editor_window), 0,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK, msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);  // destroy the window after ok is pressed
}

void show_about () {
    GtkWidget *dialog = gtk_about_dialog_new();

    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Text Editor");
    //gtk_window_set_title(GTK_WINDOW(dialog), "About Text Editor");

    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
                                  "Text Editor for IPP exercises\n");
    
    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
}

void show_help (void) {
    GtkWidget *help_window;
    GtkWidget *label;
    char help[5000];

    help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (help_window), "Help - Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(help_window), 300, 300);

    strcpy(help,
           "\nAby podłączyć usługę spell-checkera do programu trzeba:\n\n"
           "Dołączyć ją do menu 'Spell' w menubar.\n\n"
           "Pobrać zawartość bufora tekstu z edytora: całą lub fragment,\n"
           "  zapamiętując pozycję.\n\n");
    strcat(help, "\0");

    label = gtk_label_new(help);
    
    gtk_container_add(GTK_CONTAINER(help_window), label);

    gtk_widget_show_all(help_window);
}

// Zaślepeczki słownika (wchar_t i gunichar to prawie to samo)
//
// Oczywiście do zastąpienia prawdziwymi funkcjami

/*
struct dictionary
{
  int foo;
} dict;

gboolean dictionary_find(const struct dictionary *dict, const wchar_t* word) {
  // Parametr przekazany, wracamy do UTF-8
  char *uword = g_ucs4_to_utf8((gunichar *)word, -1, NULL, NULL, NULL);
  gboolean result;

  result = (strcmp(uword, "óroda") != 0);
  g_free(uword);
  return result;
}

void dictionary_hints (const struct dictionary *dict, const wchar_t* word,
                       struct word_list *list) {
  char *hints[] = {"broda", "środa", "uroda"};
  int i;

  word_list_init(list);
  for (i = 0; i < 3; i++) {
    wchar_t *item = (wchar_t *)g_utf8_to_ucs4_fast(hints[i], -1, NULL);

    word_list_add(list, item);
    g_free(item);
  }
}
*/

// Procedurka obsługi



static void WhatCheck (GtkMenuItem *item, gpointer data)
{
    if(current_dict == NULL)
    {
        info_msg("Nie załadowano żadnego słownika!");
        return;
    }
    GtkWidget *dialog;
    GtkTextIter start, end;
    char *word;
    gunichar *wword;

    // Znajdujemy pozycję kursora
    gtk_text_buffer_get_iter_at_mark(editor_buf, &start,
                                     gtk_text_buffer_get_insert(editor_buf));

    // Jeśli nie wewnątrz słowa, kończymy
    if (!gtk_text_iter_inside_word(&start)) {
        dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        "Kursor musi być w środku słowa");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Znajdujemy początek i koniec słowa, a potem samo słowo
    end = start;
    gtk_text_iter_backward_word_start(&start);
    gtk_text_iter_forward_word_end(&end);
    word = gtk_text_iter_get_text(&start, &end);

    // Zamieniamy na wide char (no prawie)
    wword = g_utf8_to_ucs4_fast(word, -1, NULL);


    // Sprawdzamy
    if (dictionary_find(current_dict, (wchar_t *)wword)) {
        dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                        "Wszystko w porządku,\nśpij spokojnie");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    else {
        // Czas korekty
        GtkWidget *vbox, *label, *combo;
        struct word_list hints;
        int i;
        wchar_t **words;

        dictionary_hints(current_dict, (wchar_t *)wword, &hints);
        words = word_list_get(&hints);
        dialog = gtk_dialog_new_with_buttons("Korekta", NULL, 0,
                                             GTK_STOCK_OK,
                                             GTK_RESPONSE_ACCEPT,
                                             GTK_STOCK_CANCEL,
                                             GTK_RESPONSE_REJECT,
                                             "Dodaj słowo",
                                             GTK_RESPONSE_APPLY,
                                             NULL);
        // W treści dialogu dwa elementy
        vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        // Tekst
        label = gtk_label_new("Coś nie tak, mam kilka propozycji");
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);

        // Spuszczane menu
        combo = gtk_combo_box_text_new();
        for (i = 0; i < word_list_size(&hints); i++) {
            // Combo box lubi mieć Gtk
            char *uword = g_ucs4_to_utf8((gunichar *)words[i], -1, NULL, NULL, NULL);

            // Dodajemy kolejny element
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), uword);
            g_free(uword);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
        gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
        gtk_widget_show(combo);

        GtkResponseType response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_ACCEPT) {
            char *korekta =
                    gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

            // Usuwamy stare
            gtk_text_buffer_delete(editor_buf, &start, &end);
            // Wstawiamy nowe
            gtk_text_buffer_insert(editor_buf, &start, korekta, -1);
            g_free(korekta);
        }
        if(response == GTK_RESPONSE_APPLY)
        {
            dictionary_insert(current_dict, (wchar_t*) wword);
            dictionary_changed = true;
            info_msg("Nowe słowo zostało dodane do słownika");
        }
        gtk_widget_destroy(dialog);
    }
    g_free(word);
    g_free(wword);
}

void save_dict_if_modified(void)
{
    if(dictionary_changed)
        dictionary_save_lang(current_dict, lang_buffer+lang_position[current_dict_pos]);
}

///data points to an int which indicates position in global array lang_buffer where
///desired dictionary name is saved.
static void load_dict_port(GtkWidget *widget, gpointer data)
{
    int pos = *(int*) data;
    if(current_dict != NULL)
    {
        save_dict_if_modified();
        dictionary_done(current_dict);
    }
    current_dict = dictionary_load_lang(lang_buffer+lang_position[pos]);
    current_dict_pos = pos;
    return;
}



// Tutaj dodacie nowe pozycje menu

void init_languages_buffer(void)
{
    if(dictionary_lang_list(&lang_buffer, &lang_list_len) < 0)
    {
        g_printerr("Cannot load languages list");
        return;
    }
    assert(lang_buffer != NULL);
    assert(lang_list_len != -1);
    assert(lang_buffer[lang_list_len-1] == 0);

    for(int i = 0; i < lang_list_len; i++)
        if(lang_buffer[i] == '\0')
            lang_count++;
    assert(lang_count > 0);
    lang_position = g_malloc(sizeof(int) * lang_count);

    //filling dictionaries adresses
    bool saved = false;
    int to_save = 0;
    for(int i = 0; i < lang_list_len; i++)
    {
        if(saved)
        {
            if(lang_buffer[i] == '\0')
                saved = false;
            else continue;
        }
        else //not saved
        {
            if(lang_buffer[i] == '\0')
                assert(false);
            else
            {
                lang_position[to_save++] = i;
                saved = true;
            }
        }
    }
    /*
    printf("DEBUG:\n");
    for(int i = 0; i < lang_list_len; i++)
    {
        if(lang_buffer[i] == 0)
        {
            printf("!");
            continue;
        }
        printf("%c", lang_buffer[i]);
    }
    printf("\n%d", lang_count);
    */

}

static void empty(void)
{
    return;
}

void extend_menu_dictionary_load(GtkWidget *menubar)
{
    init_languages_buffer();


    GtkWidget *dict_menu_item, *dict_menu, *curr_dict_item;

    dict_menu_item = gtk_menu_item_new_with_label("Dictionaries");
    dict_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(dict_menu_item), dict_menu);
    gtk_widget_show(dict_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), dict_menu_item);

    int* this_lang_number;
    for(int i = 0; i < lang_count; i++)
    {
        curr_dict_item = gtk_menu_item_new_with_label(g_strdup(lang_buffer+lang_position[i]));
        this_lang_number = g_malloc(sizeof(int));
        *this_lang_number = i;
        g_signal_connect(G_OBJECT(curr_dict_item), "activate", G_CALLBACK(load_dict_port), this_lang_number);
        gtk_menu_shell_append(GTK_MENU_SHELL(dict_menu), curr_dict_item);
        gtk_widget_show(curr_dict_item);
    }
    if(lang_count == 0)
    {
        curr_dict_item = gtk_menu_item_new_with_label("No dictionaries found");
        g_signal_connect(G_OBJECT(curr_dict_item), "activate", G_CALLBACK(empty), NULL);
        gtk_menu_shell_append(GTK_MENU_SHELL(dict_menu), curr_dict_item);
        gtk_widget_show(curr_dict_item);
    }

}

void uncolor_text(void)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(editor_buf, &start);
    gtk_text_buffer_get_end_iter(editor_buf, &end);

    gtk_text_buffer_remove_all_tags(editor_buf, &start, &end);
    return;
}

void check_buffer_correctness(void)
{
    uncolor_text();
    static bool init = true;
    if(init)
    {
        gtk_text_buffer_create_tag(editor_buf, "red_fg",
                                    "foreground", "red",
                                    "weight", PANGO_WEIGHT_BOLD, NULL);
        init = false;
    }

    if(current_dict == NULL)
    {
        info_msg("Nie załadowano żadnego słownika!");
        return;
    }
    GtkTextIter start, end;
    char *word;
    wchar_t *wword;
    gtk_text_buffer_get_start_iter(editor_buf, &end);

    bool quit = false;
    while (!quit)
    {

        if(!gtk_text_iter_forward_word_end(&end))
            quit = true;
        start = end;
        gtk_text_iter_backward_word_start(&start);
        word = gtk_text_iter_get_text(&start, &end);
        wword = (wchar_t*) g_utf8_to_ucs4_fast(word, -1, NULL);

        if(dictionary_find(current_dict, wword) == DICTIONARY_WORD_NOT_FOUND)
        {

            gtk_text_buffer_apply_tag_by_name(editor_buf, "red_fg",
                                                &start, &end);
        }
        //g_print("%s\n", word);
        g_free(word);
        g_free(wword);
    }

}



void extend_menu (GtkWidget *menubar)
{
    GtkWidget *spell_menu_item, *spell_menu, *check_item;

    spell_menu_item = gtk_menu_item_new_with_label("Spell");
    spell_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(spell_menu_item), spell_menu);
    gtk_widget_show(spell_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), spell_menu_item);

    check_item = gtk_menu_item_new_with_label("Check Word");
    g_signal_connect(G_OBJECT(check_item), "activate",
                     G_CALLBACK(WhatCheck), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), check_item);
    gtk_widget_show(check_item);

    extend_menu_dictionary_load(menubar);

    GtkWidget *color_item;

    color_item = gtk_menu_item_new_with_label("Check whole text");
    g_signal_connect(G_OBJECT(color_item), "activate", G_CALLBACK(check_buffer_correctness), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), color_item);
    gtk_widget_show(color_item);

    GtkWidget *uncolor_text_item;

    uncolor_text_item = gtk_menu_item_new_with_label("Uncolor text");
    g_signal_connect(G_OBJECT(uncolor_text_item), "activate", G_CALLBACK(uncolor_text), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), uncolor_text_item);
    gtk_widget_show(uncolor_text_item);


}



/*EOF*/
