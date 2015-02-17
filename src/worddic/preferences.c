#include "worddic.h"
#include "../common/dicfile.h"

gboolean is_update;  //update or add dic

//pref dictionaries callbacks
gboolean on_button_dictionary_remove_clicked(GtkWidget *widget, worddic *worddic) {

  GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
                                                                "listbox_dic");
  GtkListBoxRow *row = gtk_list_box_get_selected_row(listbox_dic);

  if(!row) return FALSE;
    
  //remove from the conf
  gint index = gtk_list_box_row_get_index(row);  
  GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
  
  worddic->conf->dicfile_list = g_slist_remove(worddic->conf->dicfile_list,
					       selected_element->data);
  conf_save(worddic->conf);

  //remove the row
  gtk_container_remove(listbox_dic, row);

}

gboolean on_button_dictionary_edit_clicked(GtkWidget *widget, worddic *worddic) {
  is_update = TRUE;
  GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
                                                                "listbox_dic");
  GtkListBoxRow *row = gtk_list_box_get_selected_row(listbox_dic);
  if(!row) return FALSE;
  
  gint index = gtk_list_box_row_get_index(row);  
  GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
  GjitenDicfile *dic = selected_element->data;

  //init the edit dic dialog with the selected dic name and path
  GtkDialog *dialog_dic_edit = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                        "dialog_dic_edit");

  GtkEntry* entry_edit_dic_name = gtk_builder_get_object(worddic->definitions, 
								  "entry_edit_dic_name");
  GtkFileChooserButton *fcb_edit_dic_path = NULL;
  fcb_edit_dic_path = gtk_builder_get_object(worddic->definitions, 
					     "filechooserbutton_edit_dic_path");

  gtk_entry_set_text(entry_edit_dic_name, dic->name);
  gtk_file_chooser_select_filename(fcb_edit_dic_path, dic->path);
  
  gtk_widget_show (dialog_dic_edit);
}

void display_dic_in_listbox(GtkListBox *listbox_dic, GjitenDicfile *dicfile, gint position){
  GtkBox *box_dic = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_halign(box_dic, GTK_ALIGN_START);
  
  //name of the dictionary
  GtkLabel *lab_dicname = gtk_label_new(dicfile->name);
  gtk_box_pack_start(box_dic, lab_dicname, FALSE, FALSE, 10);
    
  //path of the dictionary
  GtkLabel *lab_dicpath = gtk_label_new(dicfile->path);
  gtk_box_pack_start(box_dic, lab_dicpath, TRUE, TRUE, 0);

  //insert the listbox in the list
  gtk_list_box_insert (listbox_dic, box_dic, position);

  gtk_widget_show_all (box_dic);
}

gboolean on_button_dictionary_add_clicked(GtkWidget *widget, worddic *worddic) {
  is_update = FALSE;
  
  GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
                                                                "listbox_dic");

  //set edit dialog widgets to blank
  GtkDialog *dialog_dic_edit = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                        "dialog_dic_edit");

  GtkEntry* entry_edit_dic_name = gtk_builder_get_object(worddic->definitions, 
								  "entry_edit_dic_name");
  GtkFileChooserButton *fcb_edit_dic_path = NULL;
  fcb_edit_dic_path = gtk_builder_get_object(worddic->definitions, 
					     "filechooserbutton_edit_dic_path");

  gtk_entry_set_text(entry_edit_dic_name, "");
  gtk_file_chooser_select_filename(fcb_edit_dic_path, "");
  
  gtk_widget_show (dialog_dic_edit);
}

gboolean on_button_dic_edit_OK_clicked(GtkWidget *widget, worddic *worddic) {
  GtkDialog *dialog_dic_edit = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                        "dialog_dic_edit");
  GtkEntry* entry_edit_dic_name = gtk_builder_get_object(worddic->definitions, 
								  "entry_edit_dic_name");
  GtkFileChooserButton *fcb_edit_dic_path = NULL;
  fcb_edit_dic_path = gtk_builder_get_object(worddic->definitions, 
					     "filechooserbutton_edit_dic_path");

  
  //update or add a dictionary
  if(is_update){
    GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
								  "listbox_dic");
    GtkListBoxRow *row = gtk_list_box_get_selected_row(listbox_dic);

    //get the dictionary to update
    gint index = gtk_list_box_row_get_index(row);  
    GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
    GjitenDicfile *dicfile = selected_element->data;

    //set the new name and path
    dicfile->name = strdup(gtk_entry_get_text(entry_edit_dic_name));
    dicfile->path = gtk_file_chooser_get_filename(fcb_edit_dic_path);

    //replace the current row with a new one
    gtk_container_remove(listbox_dic, row);
    display_dic_in_listbox(listbox_dic, dicfile, index);
  }
  else{
    //add a new dictionary in the conf
    GjitenDicfile *dicfile = g_new0(GjitenDicfile, 1);
    dicfile->name = gtk_entry_get_text(entry_edit_dic_name);
    dicfile->path = gtk_file_chooser_get_filename(fcb_edit_dic_path);
    worddic->conf->dicfile_list = g_slist_append(worddic->conf->dicfile_list, dicfile);

    GtkListBox *listbox_dic = (GtkListBox*)gtk_builder_get_object(worddic->definitions, 
								  "listbox_dic");
    
    display_dic_in_listbox(listbox_dic, dicfile, -1);
  }

  GtkDialog *prefs = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                               "prefs");
  conf_save(worddic->conf);
  
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
    dicfile = dicfile_node->data;
    display_dic_in_listbox(listbox_dic, dicfile, -1);

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

