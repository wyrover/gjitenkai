#include "radical_list.h"

void radical_list_init(kanjidic *kanjidic){
  button_list = NULL;

  //populate the radical list window
  GtkGrid *grid_radical_list = (GtkGrid*)gtk_builder_get_object(kanjidic->definitions, 
                                                                "grid_radical");

  GList *radical_list = kanjidic->rad_info_list;
  radical_list = g_list_reverse(radical_list);

  gint i = 0;
  gint j = 0;

  gint last_strockes_count=0;

  for (radical_list;
       radical_list != NULL;
       radical_list = g_list_next(radical_list)) {
   
    const gchar *radical      = (const gchar*)((RadInfo*)radical_list->data)->radical;
    gint strokes_count  = ((RadInfo*)radical_list->data)->strokes;

    //the stroke count has change: display a label with the new count and
    //update the last stroke count
    if(last_strockes_count != strokes_count){
      char str_stroke[52];
      //TODO set color in pref menu
      sprintf(str_stroke, "<span font_weight='bold' fgcolor='#EE0101'>%d</span>", strokes_count);
      GtkLabel *label_stroke_count = (GtkLabel*)gtk_label_new("");
      gtk_label_set_markup(label_stroke_count, str_stroke);
      gtk_grid_attach(GTK_GRID(grid_radical_list), GTK_WIDGET(label_stroke_count), i, j, 1, 1);

      last_strockes_count = strokes_count;

      i++;
      if(i%RADICAL_PER_ROW == 0){j++;i=0;}

    }

    //add the button
    GtkButton *button_radical = (GtkButton*)gtk_button_new_with_label(radical);
    g_signal_connect(button_radical, "clicked", on_radical_button_clicked, kanjidic);
    gtk_grid_attach(GTK_GRID(grid_radical_list), GTK_WIDGET(button_radical), i, j, 1, 1);
    
    //add this button in the button list
    button_list = g_list_append(button_list, button_radical);

    i++;
    if(i%RADICAL_PER_ROW == 0){j++;i=0;}

  }
}

void radical_list_update_sensitivity(kanjidic *kanjidic){
  //get the radicals in the radical filter entry 
  GtkEntry *entry_filter_radical = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions, 
                                                                     "entry_filter_radical");

  //editable object of the radical entry on order to change the content
  GtkEditable *editable = GTK_EDITABLE(entry_filter_radical);

  //text in the radical entry
  const gchar *radicals = gtk_entry_get_text(entry_filter_radical);

  //point to the head of the button list
  GList *l = button_list;
  
  //if no radicals, set all buttons sensitivity to true
  if(!strcmp(radicals, "")){
    for (l;l != NULL;l = g_list_next(l)) {
      gtk_widget_set_sensitive(GTK_WIDGET(l->data), TRUE);
    }
    return;
  }
  
  //for every radicals, ckeck if there at least a match with the current 
  //entered radicals and the kanji button
  do{
    //get the current button and it's kanji 
    GtkButton* button = (GtkButton*)l->data;
    const gchar* cur_radical = gtk_button_get_label(button);
    
    //append the current radical to the filter entry radicals text 
    gchar* srch = g_new0(gchar, strlen(radicals) + strlen(cur_radical) + 1);
    g_strlcpy(srch, radicals, strlen(radicals) + strlen(cur_radical));
    strcat (srch, cur_radical);
    
    //if no match, set the sensitivity to false on this button
    gboolean sensitivity; 
    //TODO? create a faster function that return a boolean on the first match
    //(seems fast enought with this function but can be optimised)
    if(get_kanji_by_radical(srch, kanjidic->rad_info_hash) == NULL){
      sensitivity = FALSE;
    }
    else{
      sensitivity = TRUE;
    }
    
    gtk_widget_set_sensitive(GTK_WIDGET(button), sensitivity);

    g_free(srch);
  }while(l = g_list_next(l));

}
