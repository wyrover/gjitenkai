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
  //set the color of the color chooser button
  GtkColorChooser *color_chooser = (GtkColorChooser*)gtk_builder_get_object(worddic->definitions, 
                                                                            "colorbutton_results_highlight");
  
  gtk_color_chooser_set_rgba(color_chooser, worddic->conf->results_highlight_color);

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

  //get the textview
  GtkTextView *textview_search_results = 
    (GtkTextView*)gtk_builder_get_object(worddic->definitions, "search_results");

  //apply the newly selected font to the results textview
  gtk_widget_override_font(GTK_WIDGET(textview_search_results), font_desc);

  worddic_conf_save(worddic);
}

G_MODULE_EXPORT void on_colorbutton_results_highlight_color_set(GtkColorChooser *color_chooser, 
                                                worddic *worddic){

  gtk_color_chooser_get_rgba(color_chooser, 
                             worddic->conf->results_highlight_color);

  g_object_set(worddic->conf->highlight, "foreground-rgba",
             worddic->conf->results_highlight_color, NULL);

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
