#include "preferences.h"

gboolean is_update;  //update or add dic
gboolean done;

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
  

  if(index == -1) return FALSE;

  //get dictionary
  GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
  WorddicDicfile *dic = selected_element->data;

  //remove from memory
  worddic_dicfile_free(dic);

  //remove from the conf
  worddic->conf->dicfile_list = g_slist_remove(worddic->conf->dicfile_list, selected_element->data);
  worddic_conf_save(worddic->settings, worddic->conf);

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
  WorddicDicfile *dic = selected_element->data;  

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
  GtkEntry* entry_edit_dic_name = (GtkEntry*)gtk_builder_get_object(worddic->definitions, 
                                                                    "entry_edit_dic_name");
  GtkFileChooserButton* fcb_edit_dic_path = NULL;
  fcb_edit_dic_path = (GtkFileChooserButton*)gtk_builder_get_object(worddic->definitions, 
                                                                    "filechooserbutton_edit_dic_path");
  
  GtkTreeView *treeview_dic = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                                   "treeview_dic");
  GtkTreeIter iter ;
  WorddicDicfile *dicfile = NULL;
  
  //update or add a dictionary
  if(is_update){
    //get the dictionary to update (index according the the nth element clicked)
    gint index = getsingleselect(treeview_dic, &iter);
    GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
    dicfile = selected_element->data;

    gchar *path = g_strdup(gtk_file_chooser_get_filename((GtkFileChooser*) fcb_edit_dic_path));
    gchar *name = g_strdup(gtk_entry_get_text(entry_edit_dic_name));

    //if the dictionary is loaded, free the memory
    if(dicfile->is_loaded){
      if(!strcmp(dicfile->path, path)){
        worddic_dicfile_free_entries(dicfile);
      }
      dicfile->is_loaded = FALSE;
      dicfile->is_active = TRUE;
    }
       
    //set the new name and path
    dicfile->name = name;
    dicfile->path = path;

    
    //replace the current row with a new one
    gtk_list_store_remove(store, &iter);
  }
  else{
    //create a new dictionary and add it in the conf
    dicfile = g_new0(WorddicDicfile, 1);
    dicfile->name = g_strdup(gtk_entry_get_text(entry_edit_dic_name));
    dicfile->path = g_strdup(gtk_file_chooser_get_filename((GtkFileChooser*)fcb_edit_dic_path));
    dicfile->is_loaded = FALSE;
    dicfile->is_active = TRUE;
    worddic->conf->dicfile_list = g_slist_append(worddic->conf->dicfile_list, dicfile);
  }

  //insert a new row in the model
  gtk_list_store_insert (store, &iter, -1);
  
  //put the name and path of the dictionary in the row
  gtk_list_store_set (store, &iter,
                      COL_NAME, dicfile->name,
                      COL_PATH, dicfile->path,
                      COL_ACTIVE, dicfile->is_active,
                      COL_LOADED, dicfile->is_loaded,
                      -1);
    
  worddic_conf_save(worddic->settings, worddic->conf);

  GtkDialog *dialog_dic_edit = (GtkDialog*)gtk_builder_get_object(worddic->definitions, 
                                                                  "dialog_dic_edit");  
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

  //gloss
  //start and end entries
  GtkEntry *entry_gloss_start = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_gloss_start");
  gtk_entry_set_text(entry_gloss_start, worddic->conf->gloss.start);

  GtkEntry *entry_gloss_end = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_gloss_end");
  gtk_entry_set_text(entry_gloss_end, worddic->conf->gloss.end);

  //subgloss
  GtkFontButton *font_button_subgloss = (GtkFontButton*)
    gtk_builder_get_object(worddic->definitions, "fontbutton_subgloss");
  gtk_font_button_set_font_name (font_button_subgloss, worddic->conf->subgloss.font);

  //set the color of the color chooser button
  GtkColorChooser *color_chooser_subgloss = (GtkColorChooser*)
    gtk_builder_get_object(worddic->definitions,
                           "colorbutton_subgloss");
  
  gtk_color_chooser_set_rgba(color_chooser_subgloss,
                             worddic->conf->subgloss.color);

  //start and end entries
  GtkEntry *entry_subgloss_start = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_subgloss_start");
  gtk_entry_set_text(entry_subgloss_start, worddic->conf->subgloss.start);

  GtkEntry *entry_subgloss_end = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_subgloss_end");
  gtk_entry_set_text(entry_subgloss_end, worddic->conf->subgloss.end);

  //notes
  GtkFontButton *font_button_notes = (GtkFontButton*)
    gtk_builder_get_object(worddic->definitions, "fontbutton_notes");
  gtk_font_button_set_font_name (font_button_notes, worddic->conf->notes.font);

  //set the color of the color chooser button
  GtkColorChooser *color_chooser_notes = (GtkColorChooser*)
    gtk_builder_get_object(worddic->definitions,
                           "colorbutton_notes");
  
  gtk_color_chooser_set_rgba(color_chooser_notes,
                             worddic->conf->notes.color);

  //start and end entries
  GtkEntry *entry_notes_start = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_notes_start");
  gtk_entry_set_text(entry_notes_start, worddic->conf->notes.start);

  GtkEntry *entry_notes_end = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_notes_end");
  gtk_entry_set_text(entry_notes_end, worddic->conf->notes.end);

  ////Dictionary tab
  GtkTreeIter iter;
  GtkTreeView *view = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                           "treeview_dic");
  GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(view));

  //populate the list of dictionaries
  WorddicDicfile *dicfile;
  GSList *dicfile_node = worddic->conf->dicfile_list;
  while (dicfile_node != NULL) {
    dicfile = dicfile_node->data;
    
    //insert a new row in the model
    gtk_list_store_insert (store, &iter, -1);

    //put the name and path of the dictionary in the row
    gtk_list_store_set (store, &iter,
                        COL_NAME, dicfile->name,
                        COL_PATH, dicfile->path,
                        COL_ACTIVE, dicfile->is_active,
                        COL_LOADED, dicfile->is_loaded,
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

  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_colorbutton_results_highlight_color_set(GtkColorChooser *color_chooser, 
                                                                worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->results_highlight_color);

  g_object_set(worddic->conf->highlight, "background-rgba",
               worddic->conf->results_highlight_color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf);
}

//Definition
G_MODULE_EXPORT void on_fontbutton_jap_def_font_set(GtkFontButton *font_button, 
                                                    worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  worddic->conf->jap_def.font = font_name;

  g_object_set(worddic->conf->jap_def.tag, "font",
               worddic->conf->jap_def.font, NULL);

  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_colorbutton_jap_def_color_set(GtkColorChooser *color_chooser, 
                                                      worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->jap_def.color);

  g_object_set(worddic->conf->jap_def.tag, "foreground-rgba",
               worddic->conf->jap_def.color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_entry_jap_def_start_changed(GtkEntry *entry,
                                                    worddic *worddic){
  worddic->conf->jap_def.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_entry_jap_def_end_changed(GtkEntry *entry,
                                                  worddic *worddic){
  worddic->conf->jap_def.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);
}

//Reading
G_MODULE_EXPORT void on_fontbutton_jap_reading_font_set(GtkFontButton *font_button, 
                                                        worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  worddic->conf->jap_reading.font = font_name;

  g_object_set(worddic->conf->jap_reading.tag, "font",
               worddic->conf->jap_reading.font, NULL);
  
  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_colorbutton_jap_reading_color_set(GtkColorChooser *color_chooser, 
                                                          worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->jap_reading.color);

  g_object_set(worddic->conf->jap_reading.tag, "foreground-rgba",
               worddic->conf->jap_reading.color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_entry_jap_reading_start_changed(GtkEntry *entry,
                                                        worddic *worddic){
  worddic->conf->jap_reading.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);

}

G_MODULE_EXPORT void on_entry_jap_reading_end_changed(GtkEntry *entry,
                                                      worddic *worddic){
  worddic->conf->jap_reading.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);
}

//Gloss

G_MODULE_EXPORT void on_entry_gloss_start_changed(GtkEntry *entry,
                                                  worddic *worddic){
  worddic->conf->gloss.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_entry_gloss_end_changed(GtkEntry *entry,
                                                worddic *worddic){
  worddic->conf->gloss.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);
}


//sub gloss
G_MODULE_EXPORT void on_fontbutton_subgloss_font_set(GtkFontButton *font_button, 
                                                     worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  worddic->conf->subgloss.font = font_name;

  g_object_set(worddic->conf->subgloss.tag, "font",
               worddic->conf->subgloss.font, NULL);
  
  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_colorbutton_subgloss_color_set(GtkColorChooser *color_chooser, 
                                                       worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->subgloss.color);

  g_object_set(worddic->conf->subgloss.tag, "foreground-rgba",
               worddic->conf->subgloss.color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_entry_subgloss_start_changed(GtkEntry *entry,
                                                     worddic *worddic){
  worddic->conf->subgloss.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);

}

G_MODULE_EXPORT void on_entry_subgloss_end_changed(GtkEntry *entry,
                                                   worddic *worddic){
  worddic->conf->subgloss.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);
}

//notes
G_MODULE_EXPORT void on_fontbutton_notes_font_set(GtkFontButton *font_button, 
                                                  worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  worddic->conf->notes.font = font_name;

  g_object_set(worddic->conf->notes.tag, "font",
               worddic->conf->notes.font, NULL);

  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_colorbutton_notes_color_set(GtkColorChooser *color_chooser, 
                                                    worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->notes.color);

  g_object_set(worddic->conf->notes.tag, "foreground-rgba",
               worddic->conf->notes.color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_entry_notes_start_changed(GtkEntry *entry,
                                                  worddic *worddic){
  worddic->conf->notes.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_entry_notes_end_changed(GtkEntry *entry,
                                                worddic *worddic){
  worddic->conf->notes.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf);
}


//Search options
G_MODULE_EXPORT void on_checkbutton_search_katakana_on_hiragana_toggled(GtkCheckButton* check_button, 
                                                                        worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->search_kata_on_hira = toggled;
}

G_MODULE_EXPORT void on_checkbutton_search_hiragana_on_katakana_toggled(GtkCheckButton* check_button,  
                                                                        worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->search_hira_on_kata = toggled;

  worddic_conf_save(worddic->settings, worddic->conf);
}

G_MODULE_EXPORT void on_checkbutton_verbadj_deinflection_toggled(GtkCheckButton* check_button, 
                                                                 worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->verb_deinflection = toggled;

  worddic_conf_save(worddic->settings, worddic->conf);
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

G_MODULE_EXPORT  void on_cellrenderertoggle_active_toggled(GtkCellRendererToggle *cell,
                                                           gchar *path_str,
                                                           worddic *worddic){
  GtkListStore *model = (GtkListStore*)gtk_builder_get_object(worddic->definitions, 
                                                              "liststore_dic");
  GtkTreeIter  iter;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  gboolean active;

  //set the model
  gtk_tree_model_get_iter (GTK_TREE_MODEL(model), &iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, COL_ACTIVE, &active, -1);
  active ^= 1;
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_ACTIVE, active, -1);

  //set the conf
  gint index = gtk_tree_path_get_indices(path)[0];
  GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
  WorddicDicfile *dic = selected_element->data;
  dic->is_active = active;
  worddic_conf_save(worddic->settings, worddic->conf);
}

G_LOCK_DEFINE (fp);
gpointer proxy_worddic_dicfile_parse_all(WorddicDicfile *dicfile){
  done = FALSE;
  
  FILE* fp = dicfile->fp;
  G_LOCK (fp);
  
  worddic_dicfile_open(dicfile);
  
  //parse all entries
  worddic_dicfile_parse_all(dicfile);

  worddic_dicfile_close(dicfile);
  G_UNLOCK (fp);
  done = TRUE;

  return NULL;
}

static gboolean
cb_load_dic_timeout( dic_state_ui *ui )
{
  if(done){
    GtkCellRendererToggle *cell = ui->cell;
    GtkTreeView *tree = ui->treeview;
    
    g_object_set(cell, "activatable", TRUE, "inconsistent", FALSE, NULL);
    gtk_widget_queue_draw(GTK_WIDGET(tree));
    
    g_free(ui);
    return FALSE;
  }
  else return TRUE;
}


G_MODULE_EXPORT void on_cellrenderertoggle_loaded_toggled(GtkCellRendererToggle *cell,
                                                          gchar *path_str,
                                                          worddic *worddic){
  GtkTreeView *treeview = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                               "treeview_dic");
  GtkListStore *model = (GtkListStore*)gtk_builder_get_object(worddic->definitions, 
                                                              "liststore_dic");
  GtkTreeIter  iter;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  gboolean loaded;

  //get the loaded variable from the tree model
  gtk_tree_model_get_iter (GTK_TREE_MODEL(model), &iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, COL_LOADED, &loaded, -1);

  //get the dicfile index from UI
  gint index = gtk_tree_path_get_indices(path)[0];
  gtk_tree_path_free (path);

  //get the dicfile from index
  GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
  WorddicDicfile *dicfile = selected_element->data;

  //load in memory the entries 
  if(!loaded){
    //set the cell state activatable to false to avoid double clicks
    //and inconsistent to true to display current state
    g_object_set(cell, "activatable", FALSE, "inconsistent", TRUE, NULL);
  
    //load all dictionarie entries in a thread
    GError *error = NULL;

    GtkTreeView *treeview = (GtkTreeView*)gtk_builder_get_object(worddic->definitions, 
                                                                 "treeview_dic");
    //Create new thread
    worddic->thread_load_dic = g_thread_new ("Load dicfile",
                                             (GThreadFunc)proxy_worddic_dicfile_parse_all,
                                             dicfile);

    //update the UI every N MiliSeconds
    dic_state_ui *ui = g_new0(dic_state_ui, 1);
    ui->cell = cell;
    ui->treeview = treeview;
    gdk_threads_add_timeout( 500, (GSourceFunc)cb_load_dic_timeout, ui);
  }
  else{
    worddic_dicfile_free_entries(dicfile);
  }

  //reverse the loaded state
  loaded ^= 1;
  //save the new loaded state in the dicfile, list_store and conf
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_LOADED, loaded, -1);
  dicfile->is_loaded = loaded;
  worddic_conf_save(worddic->settings, worddic->conf);

}
