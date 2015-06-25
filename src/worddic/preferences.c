#include "preferences.h"

gboolean is_update;  //update or add dic

int getsingleselect ( GtkTreeView * tv, GtkTreeIter *iter)
{
  GtkTreeSelection * tsel = gtk_tree_view_get_selection (tv);
  GtkTreeModel * tm ;
  GtkTreePath * path ;
  int * i ;
  if ( gtk_tree_selection_get_selected ( tsel , &tm , iter ) )
    {
      path = gtk_tree_model_get_path ( tm , iter ) ;
      i = gtk_tree_path_get_indices ( path ) ;
      return i [ 0 ] ;
    }
  return -1;
}

//pref dictionaries callbacks
G_MODULE_EXPORT gboolean on_button_dictionary_remove_clicked(GtkWidget *widget, worddic *worddic) {

  GtkTreeView *treeview_dic = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                                "treeview_dic");  
  GtkTreeIter iter ;
  gint index = getsingleselect(treeview_dic, &iter);

  //remove from the conf
  if(index == -1) return FALSE;

  GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
  GjitenDicfile *dic = selected_element->data;  
  worddic->conf->dicfile_list = g_slist_remove(worddic->conf->dicfile_list, selected_element->data);
  worddic_conf_save(worddic);

  //remove from the list store
  GtkListStore *store = (GtkListStore*)gtk_builder_get_object(worddic->definitions, 
                                                              "liststore_dic");
  gtk_list_store_remove(store, &iter);
}

G_MODULE_EXPORT gboolean on_button_dictionary_edit_clicked(GtkWidget *widget, worddic *worddic) {
  is_update = TRUE;
  GtkTreeView *treeview_dic = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                                "treeview_dic");
  GtkTreeIter iter ;
  gint index = getsingleselect(treeview_dic, &iter);

  //init the edit dic dialog with the selected dic name and path
  GtkDialog *dialog_dic_edit = (GtkDialog*)gtk_builder_get_object(worddic->definitions, 
                                                        "dialog_dic_edit");

  GtkEntry* entry_edit_dic_name = (GtkEntry*)gtk_builder_get_object(worddic->definitions, 
								  "entry_edit_dic_name");
  GtkFileChooserButton *fcb_edit_dic_path = NULL;
  fcb_edit_dic_path = (GtkFileChooserButton *)gtk_builder_get_object(worddic->definitions, 
                                                                     "filechooserbutton_edit_dic_path");

  GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
  GjitenDicfile *dic = selected_element->data;  

  gtk_entry_set_text(entry_edit_dic_name, dic->name);
  gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(fcb_edit_dic_path), dic->path);
  
  gtk_widget_show (GTK_WIDGET(dialog_dic_edit));
}

G_MODULE_EXPORT gboolean on_button_dictionary_add_clicked(GtkWidget *widget, worddic *worddic) {
  is_update = FALSE;
  
  GtkTreeView *treeview_dic = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                                   "treeview_dic");
  //set edit dialog widgets to blank
  GtkDialog* dialog_dic_edit = (GtkDialog*)gtk_builder_get_object(worddic->definitions, 
                                                                  "dialog_dic_edit");

  GtkEntry* entry_edit_dic_name = (GtkEntry*)gtk_builder_get_object(worddic->definitions, 
                                                                    "entry_edit_dic_name");
  GtkFileChooserButton* fcb_edit_dic_path = NULL;
  fcb_edit_dic_path = (GtkFileChooserButton*)gtk_builder_get_object(worddic->definitions, 
                                                                    "filechooserbutton_edit_dic_path");

  //clear the edit dialog widgets
  gtk_entry_set_text(entry_edit_dic_name, "");
  gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(fcb_edit_dic_path), "");
  
  gtk_widget_show (GTK_WIDGET(dialog_dic_edit));
}

G_MODULE_EXPORT gboolean on_button_dic_edit_OK_clicked(GtkWidget *widget, worddic *worddic) {
  GtkListStore *store = (GtkListStore*)gtk_builder_get_object(worddic->definitions, 
                                                              "liststore_dic");
  GtkDialog *dialog_dic_edit = (GtkDialog*)gtk_builder_get_object(worddic->definitions, 
                                                                  "dialog_dic_edit");
  GtkEntry* entry_edit_dic_name = (GtkEntry*)gtk_builder_get_object(worddic->definitions, 
                                                                    "entry_edit_dic_name");
  GtkFileChooserButton* fcb_edit_dic_path = NULL;
  fcb_edit_dic_path = (GtkFileChooserButton*)gtk_builder_get_object(worddic->definitions, 
                                                                    "filechooserbutton_edit_dic_path");
  
  GtkTreeView *treeview_dic = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                                "treeview_dic");  
  GtkTreeIter iter ;
  gint index = getsingleselect(treeview_dic, &iter);

  //update or add a dictionary
  if(is_update){
    //get the dictionary to update
    GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
    GjitenDicfile *dicfile = selected_element->data;

    //set the new name and path
    dicfile->name = strdup(gtk_entry_get_text(entry_edit_dic_name));
    dicfile->path = gtk_file_chooser_get_filename((GtkFileChooser*) fcb_edit_dic_path);

    //replace the current row with a new one
    gtk_list_store_remove(store, &iter);

    //insert a new row in the model
    gtk_list_store_insert (store, &iter, -1);

    //put the name and path of the dictionary in the row
    gtk_list_store_set (store, &iter,
                        COL_NAME, dicfile->name,
                        COL_PATH, dicfile->path,
                        -1);
  }
  else{
    //add a new dictionary in the conf
    GjitenDicfile *dicfile = g_new0(GjitenDicfile, 1);
    dicfile->name = gtk_entry_get_text(entry_edit_dic_name);
    dicfile->path = gtk_file_chooser_get_filename((GtkFileChooser*)fcb_edit_dic_path);
    worddic->conf->dicfile_list = g_slist_append(worddic->conf->dicfile_list, dicfile);

    //add in the tree
    GtkTreeView *treeview_dic = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                                     "treeview_dic");
    GtkListStore *store = (GtkListStore*)gtk_builder_get_object(worddic->definitions, 
                                                                "liststore_dic");
    GtkTreeIter iter;

    //insert a new row in the model
    gtk_list_store_insert (store, &iter, -1);

    //put the name and path of the dictionary in the row
    gtk_list_store_set (store, &iter,
                        COL_NAME, dicfile->name,
                        COL_PATH, dicfile->path,
                        -1);
    }

  GtkDialog *prefs = (GtkDialog*)gtk_builder_get_object(worddic->definitions, 
                                                        "prefs");
  worddic_conf_save(worddic);
  
  gtk_widget_hide (GTK_WIDGET(dialog_dic_edit));
}

void init_prefs_window(worddic *worddic){
  ////appearance tab

  //set the font button
  //init the kanji font chooser
  GtkFontButton *font_button = (GtkFontButton*)gtk_builder_get_object(worddic->definitions, 
                                                                      "fontbutton_results");
  gtk_font_button_set_font_name (font_button, worddic->conf->resultsfont);

  //set the color of the color chooser button
  GtkColorChooser *color_chooser = (GtkColorChooser*)
    gtk_builder_get_object(worddic->definitions,
                           "colorbutton_results_highlight");
  
  gtk_color_chooser_set_rgba(color_chooser,
                             worddic->conf->results_highlight_color);

  //japanese definition
  GtkFontButton *font_button_jap_def = (GtkFontButton*)
    gtk_builder_get_object(worddic->definitions, "fontbutton_jap_def");
  gtk_font_button_set_font_name (font_button_jap_def, worddic->conf->jap_def.font);

  //set the color of the color chooser button
  GtkColorChooser *color_chooser_jap_def = (GtkColorChooser*)
    gtk_builder_get_object(worddic->definitions,
                           "colorbutton_jap_def");
  
  gtk_color_chooser_set_rgba(color_chooser_jap_def,
                             worddic->conf->jap_def.color);

  //start and end entries
  GtkEntry *entry_jap_def_start = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_jap_def_start");
  gtk_entry_set_text(entry_jap_def_start, worddic->conf->jap_def.start);

  GtkEntry *entry_jap_def_end = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_jap_def_end");
  gtk_entry_set_text(entry_jap_def_end, worddic->conf->jap_def.end);

  //japanese reading
  GtkFontButton *font_button_jap_reading = (GtkFontButton*)
    gtk_builder_get_object(worddic->definitions, "fontbutton_jap_reading");
  gtk_font_button_set_font_name (font_button_jap_reading, worddic->conf->jap_reading.font);

  //set the color of the color chooser button
  GtkColorChooser *color_chooser_jap_reading = (GtkColorChooser*)
    gtk_builder_get_object(worddic->definitions,
                           "colorbutton_jap_reading");
  
  gtk_color_chooser_set_rgba(color_chooser_jap_reading,
                             worddic->conf->jap_reading.color);

  //start and end entries
  GtkEntry *entry_jap_reading_start = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_jap_reading_start");
  gtk_entry_set_text(entry_jap_reading_start, worddic->conf->jap_reading.start);

  GtkEntry *entry_jap_reading_end = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_jap_reading_end");
  gtk_entry_set_text(entry_jap_reading_end, worddic->conf->jap_reading.end);

  //translations
  GtkFontButton *font_button_gloss = (GtkFontButton*)
    gtk_builder_get_object(worddic->definitions, "fontbutton_translation");
  gtk_font_button_set_font_name (font_button_gloss, worddic->conf->gloss.font);

  //set the color of the color chooser button
  GtkColorChooser *color_chooser_translation = (GtkColorChooser*)
    gtk_builder_get_object(worddic->definitions,
                           "colorbutton_translation");
  
  gtk_color_chooser_set_rgba(color_chooser_translation,
                             worddic->conf->gloss.color);

  //start and end entries
  GtkEntry *entry_translation_start = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_translation_start");
  gtk_entry_set_text(entry_translation_start, worddic->conf->gloss.start);

  GtkEntry *entry_translation_end = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_translation_end");
  gtk_entry_set_text(entry_translation_end, worddic->conf->gloss.end);

  
  ////Dictionary tab
  GtkListStore *store = (GtkListStore*)gtk_builder_get_object(worddic->definitions, 
                                                              "liststore_dic");

  GtkTreeView *view = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                           "treeview_dic");


  GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Name",  
                                               renderer,
                                               "text", COL_NAME,
                                               NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Path",  
                                               renderer,
                                               "text", COL_PATH,
                                               NULL);
  GtkTreeIter iter;
    
  //populate the list of dictionaries with widgets to edit/remove them
  GjitenDicfile *dicfile;
  GSList *dicfile_node = worddic->conf->dicfile_list;
  while (dicfile_node != NULL) {
    dicfile = dicfile_node->data;

    //insert a new row in the model
    gtk_list_store_insert (store, &iter, -1);

    //put the name and path of the dictionary in the row
    gtk_list_store_set (store, &iter,
                        COL_NAME, dicfile->name,
                        COL_PATH, dicfile->path,
                        -1);

    dicfile_node = g_slist_next(dicfile_node);
  }
  

  ////Search tab 
  GtkToggleButton *check_button;
  check_button = (GtkToggleButton*)gtk_builder_get_object(worddic->definitions, 
                                                          "checkbutton_verbadj_deinflection");
  gtk_toggle_button_set_active(check_button, worddic->conf->verb_deinflection);

  check_button = (GtkToggleButton*)gtk_builder_get_object(worddic->definitions, 
                                                          "checkbutton_search_hiragana_on_katakana");
  gtk_toggle_button_set_active(check_button, worddic->conf->search_hira_on_kata);

  check_button = (GtkToggleButton*)gtk_builder_get_object(worddic->definitions, 
                                                          "checkbutton_search_katakana_on_hiragana");

  gtk_toggle_button_set_active(check_button, worddic->conf->search_kata_on_hira);
}


G_MODULE_EXPORT void on_fontbutton_results_font_set(GtkFontButton *font_button, 
                                                    worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  worddic->conf->resultsfont = font_name;

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_colorbutton_results_highlight_color_set(GtkColorChooser *color_chooser, 
                                                worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->results_highlight_color);

  g_object_set(worddic->conf->highlight, "background-rgba",
             worddic->conf->results_highlight_color, NULL);

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_fontbutton_jap_def_font_set(GtkFontButton *font_button, 
                                                    worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  worddic->conf->jap_def.font = font_name;

  g_object_set(worddic->conf->jap_def.tag, "font",
               worddic->conf->jap_def.font, NULL);

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_colorbutton_jap_def_color_set(GtkColorChooser *color_chooser, 
                                                worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->jap_def.color);

  g_object_set(worddic->conf->jap_def.tag, "foreground-rgba",
             worddic->conf->jap_def.color, NULL);

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_entry_jap_def_start_changed(GtkEntry *entry,
                                                    worddic *worddic){
  worddic->conf->jap_def.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_entry_jap_def_end_changed(GtkEntry *entry,
                                                    worddic *worddic){
    worddic->conf->jap_def.end = gtk_entry_get_text(entry);
    worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_fontbutton_jap_reading_font_set(GtkFontButton *font_button, 
                                                    worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  worddic->conf->jap_reading.font = font_name;

  g_object_set(worddic->conf->jap_reading.tag, "font",
             worddic->conf->jap_reading.font, NULL);
  
  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_colorbutton_jap_reading_color_set(GtkColorChooser *color_chooser, 
                                                worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->jap_reading.color);

  g_object_set(worddic->conf->jap_reading.tag, "foreground-rgba",
             worddic->conf->jap_reading.color, NULL);

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_entry_jap_reading_start_changed(GtkEntry *entry,
                                                    worddic *worddic){
  worddic->conf->jap_reading.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic);

}

G_MODULE_EXPORT void on_entry_jap_reading_end_changed(GtkEntry *entry,
                                                    worddic *worddic){
    worddic->conf->jap_reading.end = gtk_entry_get_text(entry);
    worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_fontbutton_translation_font_set(GtkFontButton *font_button, 
                                                    worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  worddic->conf->gloss.font = font_name;

  g_object_set(worddic->conf->gloss.tag, "font",
               worddic->conf->gloss.font, NULL);

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_colorbutton_translation_color_set(GtkColorChooser *color_chooser, 
                                                worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->gloss.color);

  g_object_set(worddic->conf->gloss.tag, "foreground-rgba",
             worddic->conf->gloss.color, NULL);

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_entry_translation_start_changed(GtkEntry *entry,
                                                    worddic *worddic){
  worddic->conf->gloss.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_entry_translation_end_changed(GtkEntry *entry,
                                                    worddic *worddic){
  worddic->conf->gloss.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_checkbutton_search_katakana_on_hiragana_toggled(GtkCheckButton* check_button, 
                                                        worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->search_kata_on_hira = toggled;
}

G_MODULE_EXPORT void on_checkbutton_search_hiragana_on_katakana_toggled(GtkCheckButton* check_button,  
                                                worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->search_hira_on_kata = toggled;

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_checkbutton_verbadj_deinflection_toggled(GtkCheckButton* check_button, 
                                                                 worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->verb_deinflection = toggled;

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT gboolean on_button_OK_clicked(GtkWidget *widget, worddic *worddic) {
  GtkDialog *prefs = (GtkDialog*)gtk_builder_get_object(worddic->definitions, 
                                                        "prefs");
  gtk_widget_hide (GTK_WIDGET(prefs));
}

//hide and prevent deletion
G_MODULE_EXPORT gboolean on_dialog_dic_edit_delete_event(GtkWindow *window, 
                                                         worddic *worddic) {
  gtk_widget_hide(GTK_WIDGET(window));
  return TRUE;

}

//hide and prevent deletion
G_MODULE_EXPORT gboolean on_prefs_delete_event(GtkWindow *window, 
                                               worddic *worddic) {
  gtk_widget_hide(GTK_WIDGET(window));
  return TRUE;
}
