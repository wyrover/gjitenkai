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
G_MODULE_EXPORT gboolean on_button_dictionary_remove_clicked(GtkWidget *widget,
                                                             worddic *worddic) {

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
  worddic_conf_save(worddic->settings, worddic->conf, WSE_DICFILE);

  //remove from the list store
  GtkListStore *store = (GtkListStore*)gtk_builder_get_object(worddic->definitions,
                                                              "liststore_dic");
  gtk_list_store_remove(store, &iter);
  return TRUE;
}

G_MODULE_EXPORT gboolean on_button_dictionary_edit_clicked(GtkWidget *widget, worddic *worddic) {
  is_update = TRUE;
  GtkTreeView *treeview_dic = (GtkTreeView*)gtk_builder_get_object(worddic->definitions,
                                                                   "treeview_dic");
  GtkTreeIter iter ;
  gint index = getsingleselect(treeview_dic, &iter);

  if(index == -1)return FALSE;

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
  return TRUE;
}

G_MODULE_EXPORT gboolean on_button_dictionary_add_clicked(GtkWidget *widget, worddic *worddic) {
  is_update = FALSE;

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
  return TRUE;
}

void add_new_dictionary(gchar *name, gchar *path, worddic* worddic){

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
  GtkTreeIter iter;
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
  }
  else{
    //create a new dictionary and add it in the conf
    dicfile = g_new0(WorddicDicfile, 1);
    dicfile->name = g_strdup(gtk_entry_get_text(entry_edit_dic_name));
    dicfile->path = g_strdup(gtk_file_chooser_get_filename((GtkFileChooser*)fcb_edit_dic_path));
    dicfile->is_loaded = FALSE;
    dicfile->is_active = TRUE;
    worddic->conf->dicfile_list = g_slist_append(worddic->conf->dicfile_list, dicfile);

    //insert a new row in the model
    gtk_list_store_insert (store, &iter, -1);

  }

  //update the model
  gtk_list_store_set (store, &iter,
                      COL_NAME, dicfile->name,
                      COL_PATH, dicfile->path,
                      COL_ACTIVE, dicfile->is_active,
                      COL_LOADED, dicfile->is_loaded,
                      -1);


  worddic_conf_save(worddic->settings, worddic->conf, WSE_DICFILE);

  GtkDialog *dialog_dic_edit = (GtkDialog*)gtk_builder_get_object(worddic->definitions,
                                                                  "dialog_dic_edit");
  gtk_widget_hide (GTK_WIDGET(dialog_dic_edit));
  return TRUE;
}

void init_prefs_window(worddic *worddic){
  ////appearance tab

  //init the kanji font chooser
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

  //sense
  //start and end entries
  GtkEntry *entry_sense_start = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_gloss_start");
  gtk_entry_set_text(entry_sense_start, worddic->conf->sense.start);

  GtkEntry *entry_sense_end = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_gloss_end");
  gtk_entry_set_text(entry_sense_end, worddic->conf->sense.end);

  //subsense
  GtkFontButton *font_button_subsense = (GtkFontButton*)
    gtk_builder_get_object(worddic->definitions, "fontbutton_subsense");
  gtk_font_button_set_font_name (font_button_subsense, worddic->conf->subsense.font);

  //set the color of the color chooser button
  GtkColorChooser *color_chooser_subsense = (GtkColorChooser*)
    gtk_builder_get_object(worddic->definitions,
                           "colorbutton_subsense");

  gtk_color_chooser_set_rgba(color_chooser_subsense,
                             worddic->conf->subsense.color);

  //start and end entries
  GtkEntry *entry_subsense_start = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_subgloss_start");
  gtk_entry_set_text(entry_subsense_start, worddic->conf->subsense.start);

  GtkEntry *entry_subsense_end = (GtkEntry*)
    gtk_builder_get_object(worddic->definitions,
                           "entry_subgloss_end");
  gtk_entry_set_text(entry_subsense_end, worddic->conf->subsense.end);

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

  //dark theme
  GtkWidget *checkbutton_dark_theme = (GtkWidget *)gtk_builder_get_object(worddic->definitions,
                                                                          "checkbutton_dark_theme");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_dark_theme),
                               worddic->conf->dark_theme);
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

  check_button = (GtkToggleButton*)gtk_builder_get_object(worddic->definitions,
                                                          "checkbutton_record_history");
  gtk_toggle_button_set_active(check_button, worddic->conf->record_history);
}


G_MODULE_EXPORT void on_colorbutton_results_highlight_color_set(GtkColorChooser *color_chooser,
                                                                worddic *worddic){
  gtk_color_chooser_get_rgba(color_chooser,
                             worddic->conf->results_highlight_color);

  g_object_set(worddic->conf->highlight, "background-rgba",
               worddic->conf->results_highlight_color, NULL);

  worddic_conf_save(worddic->settings,
                    worddic->conf,
                    WSE_HIGHLIGHT_COLOR);
}

//Definition
G_MODULE_EXPORT void on_fontbutton_jap_def_font_set(GtkFontButton *font_button,
                                                    worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  worddic->conf->jap_def.font = font_name;

  g_object_set(worddic->conf->jap_def.tag, "font",
               worddic->conf->jap_def.font, NULL);

  worddic_conf_save(worddic->settings, worddic->conf, WSE_JAPANESE_DEFINITION);
}

G_MODULE_EXPORT void on_colorbutton_jap_def_color_set(GtkColorChooser *color_chooser,
                                                      worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser,
                             worddic->conf->jap_def.color);

  g_object_set(worddic->conf->jap_def.tag, "foreground-rgba",
               worddic->conf->jap_def.color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf, WSE_JAPANESE_DEFINITION);
}

G_MODULE_EXPORT void on_entry_jap_def_start_changed(GtkEntry *entry,
                                                    worddic *worddic){
  worddic->conf->jap_def.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_JAPANESE_DEFINITION);
}

G_MODULE_EXPORT void on_entry_jap_def_end_changed(GtkEntry *entry,
                                                  worddic *worddic){
  worddic->conf->jap_def.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_JAPANESE_DEFINITION);
}

//Reading
G_MODULE_EXPORT void on_fontbutton_jap_reading_font_set(GtkFontButton *font_button,
                                                        worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);

  worddic->conf->jap_reading.font = font_name;

  g_object_set(worddic->conf->jap_reading.tag, "font",
               worddic->conf->jap_reading.font, NULL);

  worddic_conf_save(worddic->settings, worddic->conf, WSE_JAPANESE_READING);
}

G_MODULE_EXPORT void on_colorbutton_jap_reading_color_set(GtkColorChooser *color_chooser,
                                                          worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser,
                             worddic->conf->jap_reading.color);

  g_object_set(worddic->conf->jap_reading.tag, "foreground-rgba",
               worddic->conf->jap_reading.color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf, WSE_JAPANESE_READING);
}

G_MODULE_EXPORT void on_entry_jap_reading_start_changed(GtkEntry *entry,
                                                        worddic *worddic){
  worddic->conf->jap_reading.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_JAPANESE_READING);

}

G_MODULE_EXPORT void on_entry_jap_reading_end_changed(GtkEntry *entry,
                                                      worddic *worddic){
  worddic->conf->jap_reading.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_JAPANESE_READING);
}

//Sense

G_MODULE_EXPORT void on_entry_sense_start_changed(GtkEntry *entry,
                                                  worddic *worddic){
  worddic->conf->sense.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_SENSE);
}

G_MODULE_EXPORT void on_entry_sense_end_changed(GtkEntry *entry,
                                                worddic *worddic){
  worddic->conf->sense.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_SENSE);
}


//sub sense
G_MODULE_EXPORT void on_fontbutton_subsense_font_set(GtkFontButton *font_button,
                                                     worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);

  worddic->conf->subsense.font = font_name;

  g_object_set(worddic->conf->subsense.tag, "font",
               worddic->conf->subsense.font, NULL);

  worddic_conf_save(worddic->settings, worddic->conf, WSE_SENSE);
}

G_MODULE_EXPORT void on_colorbutton_subsense_color_set(GtkColorChooser *color_chooser,
                                                       worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser,
                             worddic->conf->subsense.color);

  g_object_set(worddic->conf->subsense.tag, "foreground-rgba",
               worddic->conf->subsense.color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf, WSE_SENSE);
}

G_MODULE_EXPORT void on_entry_subsense_start_changed(GtkEntry *entry,
                                                     worddic *worddic){
  worddic->conf->subsense.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_SENSE);

}

G_MODULE_EXPORT void on_entry_subsense_end_changed(GtkEntry *entry,
                                                   worddic *worddic){
  worddic->conf->subsense.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_SENSE);
}

//notes
G_MODULE_EXPORT void on_fontbutton_notes_font_set(GtkFontButton *font_button,
                                                  worddic *worddic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  worddic->conf->notes.font = font_name;

  g_object_set(worddic->conf->notes.tag, "font",
               worddic->conf->notes.font, NULL);

  worddic_conf_save(worddic->settings, worddic->conf, WSE_NOTES);
}

G_MODULE_EXPORT void on_colorbutton_notes_color_set(GtkColorChooser *color_chooser,
                                                    worddic *worddic){
  gtk_color_chooser_get_rgba(color_chooser,
                             worddic->conf->notes.color);

  g_object_set(worddic->conf->notes.tag, "foreground-rgba",
               worddic->conf->notes.color, NULL);

  worddic_conf_save(worddic->settings, worddic->conf, WSE_NOTES);
}

G_MODULE_EXPORT void on_entry_notes_start_changed(GtkEntry *entry,
                                                  worddic *worddic){
  worddic->conf->notes.start = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_NOTES);
}

G_MODULE_EXPORT void on_entry_notes_end_changed(GtkEntry *entry,
                                                worddic *worddic){
  worddic->conf->notes.end = gtk_entry_get_text(entry);
  worddic_conf_save(worddic->settings, worddic->conf, WSE_NOTES);
}


//Search options
G_MODULE_EXPORT void on_checkbutton_search_katakana_on_hiragana_toggled(GtkCheckButton* check_button,
                                                                        worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->search_kata_on_hira = toggled;
  worddic_conf_save(worddic->settings, worddic->conf, WSE_SEARCH_OPTION);
}

G_MODULE_EXPORT void on_checkbutton_search_hiragana_on_katakana_toggled(GtkCheckButton* check_button,
                                                                        worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->search_hira_on_kata = toggled;

  worddic_conf_save(worddic->settings, worddic->conf, WSE_SEARCH_OPTION);
}

G_MODULE_EXPORT void on_checkbutton_verbadj_deinflection_toggled(GtkCheckButton* check_button,
                                                                 worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->verb_deinflection = toggled;

  worddic_conf_save(worddic->settings, worddic->conf, WSE_SEARCH_OPTION);
}

G_MODULE_EXPORT void on_checkbutton_record_history_toggled(GtkCheckButton* check_button,
                                                               worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);
  worddic->conf->record_history = toggled;

  worddic_conf_save(worddic->settings, worddic->conf, WSE_SEARCH_OPTION);
}

G_MODULE_EXPORT gboolean on_button_OK_clicked(GtkWidget *widget,
                                              worddic *worddic) {
  GtkDialog *prefs = (GtkDialog*)gtk_builder_get_object(worddic->definitions,
                                                        "prefs");
  gtk_widget_hide (GTK_WIDGET(prefs));
  return TRUE;
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
  worddic_conf_save(worddic->settings, worddic->conf, WSE_DICFILE);
}

static gboolean cb_load_dic_timeout( dic_state_ui *ui )
{

  if(!ui->dicfile->is_valid){
    GtkCellRendererToggle *cell = ui->cell;
    GtkTreeView *tree = ui->treeview;

    g_object_set(cell, "activatable", TRUE, "inconsistent", FALSE, NULL);
    gtk_widget_queue_draw(GTK_WIDGET(tree));

    g_thread_unref(ui->worddic->thread_load_dic);
    ui->worddic->thread_load_dic = NULL;
    g_free(ui);
    return FALSE;
  }
  else if(ui->dicfile->is_loaded){
    GtkCellRendererToggle *cell = ui->cell;
    GtkTreeView *tree = ui->treeview;

    g_object_set(cell, "activatable", TRUE, "inconsistent", FALSE, NULL);
    gtk_widget_queue_draw(GTK_WIDGET(tree));

    if(ui->dicfile->type && ui->dicfile->copyright && ui->dicfile->creation_date){
      const char *format = "<span style=\"italic\">\%s</span> %s %s";
      char *markup = g_markup_printf_escaped (format,
					      ui->dicfile->type,
					      ui->dicfile->copyright,
					      ui->dicfile->creation_date);

      gtk_label_set_markup(ui->label_dic_info, markup);
      g_free(markup);
    }
    else if(ui->dicfile->informations){
      gtk_label_set_text(ui->label_dic_info, ui->dicfile->informations);
    }

    g_thread_unref(ui->worddic->thread_load_dic);
    ui->worddic->thread_load_dic = NULL;
    g_free(ui);
    return FALSE;
  }
  else return TRUE;
}


G_MODULE_EXPORT void on_cellrenderertoggle_loaded_toggled(GtkCellRendererToggle *cell,
                                                          gchar *path_str,
                                                          worddic *worddic){
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
    //and set the loaded status to inconsistent
    g_object_set(cell, "activatable", FALSE, "inconsistent", TRUE, NULL);

    GtkTreeView *treeview = (GtkTreeView*)gtk_builder_get_object(worddic->definitions,
                                                                 "treeview_dic");
    //Create new thread
    worddic->thread_load_dic = g_thread_new ("Load dicfile",
                                             (GThreadFunc)worddic_dicfile_open_parse_all_close,
                                             dicfile);

    //update the UI every N MiliSeconds
    GtkLabel *label_dic_info = (GtkLabel*)gtk_builder_get_object(worddic->definitions,
                                                                 "label_dic_info");
    dic_state_ui *ui = g_new0(dic_state_ui, 1);
    ui->cell = cell;
    ui->treeview = treeview;
    ui->dicfile = dicfile;
    ui->label_dic_info = label_dic_info;
    ui->worddic = worddic;
    gdk_threads_add_timeout( 500, (GSourceFunc)cb_load_dic_timeout, ui);
  }
  else{
    worddic_dicfile_free_entries(dicfile);
    dicfile->is_loaded = FALSE;
  }

  //reverse the loaded state
  loaded ^= 1;

  //save the new loaded state in the liststore
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_LOADED, loaded, -1);
}


G_MODULE_EXPORT gboolean on_treeview_dicfile_changed(GtkTreeSelection *treeselection,
                                                     worddic *worddic){
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  gchar *PATH;

  if (!gtk_tree_selection_get_selected(treeselection, &model, &iter))return FALSE;

  gtk_tree_model_get (model, &iter, COL_PATH, &PATH, -1);
  GtkLabel *label_dic_info = (GtkLabel*)gtk_builder_get_object(worddic->definitions,
                                                               "label_dic_info");

  GtkTreeView *treeview_dic = gtk_tree_selection_get_tree_view(treeselection);
  gint index = getsingleselect(treeview_dic, &iter);

  GSList *selected_element = g_slist_nth(worddic->conf->dicfile_list, index);
  WorddicDicfile *dic = selected_element->data;

  if(dic->type && dic->copyright && dic->creation_date){
    const char *format = "<span style=\"italic\">\%s</span> %s %s";
    char *markup = g_markup_printf_escaped (format, dic->type, dic->copyright, dic->creation_date);

    gtk_label_set_markup(label_dic_info, markup);
    g_free(markup);
  }
  else if(dic->informations){
    gtk_label_set_text(label_dic_info, dic->informations);
  }
  else{
    gtk_label_set_text(label_dic_info, PATH);
  }

  return TRUE;
}
