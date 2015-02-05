#include "worddic.h"
#include "../common/dicfile.h"

//pref dictionaries callbacks
gboolean on_button_dictionary_remove_clicked(GtkWidget *widget, worddic *worddic) {
  GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
                                                                "listbox_dic");
  GtkListBoxRow *row = gtk_list_box_get_selected_row(listbox_dic);
  gtk_container_remove(listbox_dic, row);

}

gboolean on_button_dictionary_edit_clicked(GtkWidget *widget, worddic *worddic) {
  GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
                                                                "listbox_dic");
  GtkListBoxRow *row = gtk_list_box_get_selected_row(listbox_dic);
  GtkWidget *children = gtk_container_get_children(row);

  gchar *t = gtk_label_get_text(children);
  g_printf("->%s\n", t);

  GtkDialog *dialog_dic_edit = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                        "dialog_dic_edit");
  gtk_widget_show (dialog_dic_edit);
}

gboolean on_button_dictionary_add_clicked(GtkWidget *widget, worddic *worddic) {
  GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
                                                                "listbox_dic");

  GtkDialog *dialog_dic_edit = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                        "dialog_dic_edit");

    gtk_widget_show (dialog_dic_edit);
}

gboolean on_button_dic_edit_OK_clicked(GtkWidget *widget, worddic *worddic) {
  GtkDialog *dialog_dic_edit = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                        "dialog_dic_edit");
  gtk_widget_hide (dialog_dic_edit);
}

void init_prefs_window(worddic *worddic){
  ////appearance tab
  //set the color of the color chooser button
  GtkColorChooser *color_chooser = (GtkColorChooser*)gtk_builder_get_object(worddic->definitions, 
                                                                            "colorbutton_results_highlight");
  
  gtk_color_chooser_set_rgba(color_chooser, worddic->conf->results_highlight_color);

  //set the font in the font chooser button
  /*GtkFontButton *fontbutton_results = (GtkFontButton*)gtk_builder_get_object(worddic->definitions,
                                                                             "fontbutton_results");
  gtk_font_button_set_font_name(fontbutton_results, worddic->conf->resultsfont);
  */

  ////Dictionary tab
  GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
                                                                "listbox_dic");

  //populate the list of dictionaries with widgets to edit/remove them
  GjitenDicfile *dicfile;
  GSList *dicfile_node = worddic->conf->dicfile_list;
  while (dicfile_node != NULL) {
    if (dicfile_node->data == NULL) break;

    dicfile = dicfile_node->data;
    GtkBox *box_dic = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    //name of the dictionary
    GtkLabel *lab_dicname = gtk_label_new(dicfile->name);
    gtk_box_pack_start(box_dic, lab_dicname, TRUE, TRUE, 0);

    //path of the dictionary
    GtkLabel *lab_dicpath = gtk_label_new(dicfile->path);
    gtk_label_set_justify(GTK_LABEL(lab_dicpath), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(box_dic, lab_dicpath, TRUE, TRUE, 0);

    //insert the listbox in the list
    gtk_list_box_insert (listbox_dic, box_dic, -1);

    dicfile_node = g_slist_next(dicfile_node);
  }

  ////Search tab 
  GtkToggleButton *check_button;
  check_button = gtk_builder_get_object(worddic->definitions, 
					"checkbutton_verbadj_deinflection");
  gtk_toggle_button_set_active(check_button, worddic->conf->verb_deinflection);

  check_button = gtk_builder_get_object(worddic->definitions, 
					"checkbutton_search_hiragana_on_katakana");
  gtk_toggle_button_set_active(check_button, worddic->conf->search_hira_on_kata);

  check_button = gtk_builder_get_object(worddic->definitions, 
					"checkbutton_search_katakana_on_hiragana");

  gtk_toggle_button_set_active(check_button, worddic->conf->search_kata_on_hira);
}


void on_fontbutton_results_font_set(GtkFontButton *font_button, 
                                    worddic *worddic){
  gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  //get the textview
  GtkTextView *textview_search_results = 
    (GtkTextBuffer*)gtk_builder_get_object(worddic->definitions, "search_results");

  //apply the newly selected font to the results textview
  gtk_widget_modify_font(textview_search_results, font_desc);

  //TODO conf set font 

  conf_save(worddic->conf);
}

void on_colorbutton_results_highlight_color_set(GtkColorChooser *color_chooser, 
                                                worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->results_highlight_color);

  g_object_set(worddic->conf->highlight, "foreground-rgba",
             worddic->conf->results_highlight_color, NULL);

  conf_save(worddic->conf);
}

void on_checkbutton_search_katakana_on_hiragana_toggled(GtkCheckButton* check_button, 
                                                        worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->search_kata_on_hira = toggled;
}

void on_checkbutton_search_hiragana_on_katakana_toggled(GtkCheckButton* check_button,  
                                                worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->search_hira_on_kata = toggled;

  conf_save(worddic->conf);
}

void on_checkbutton_verbadj_deinflection_toggled(GtkCheckButton* check_button, 
                                                worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->verb_deinflection = toggled;

  conf_save(worddic->conf);
}

gboolean on_button_OK_clicked(GtkWidget *widget, worddic *worddic) {
  GtkDialog *prefs = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                        "prefs");
  gtk_widget_hide (prefs);
}

