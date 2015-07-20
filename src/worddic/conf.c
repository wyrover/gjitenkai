#include "conf.h"

WorddicConfig *worddic_conf_load(GSettings *settings){
  WorddicConfig *conf;

  conf = g_new0(WorddicConfig, 1);

  //load the font familly for the results
  conf->resultsfont = g_settings_get_string(settings, "resultsfont");

  //load the results highlight color from string
  char *str_results_highlight_color = g_settings_get_string(settings, "results-highlight-color");

  //load the dictionary entries units styles
  ////japanese definition
  conf->jap_def.start = g_settings_get_string(settings, "japanese-definition-start");
  conf->jap_def.end = g_settings_get_string(settings, "japanese-definition-end");
  conf->jap_def.font = g_settings_get_string(settings, "japanese-definition-font");
  gchar *jap_def_color_str = g_settings_get_string(settings, "japanese-definition-color");
  conf->jap_def.color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->jap_def.color, jap_def_color_str);

  ////japanese reading
  conf->jap_reading.start = g_settings_get_string(settings, "japanese-reading-start");
  conf->jap_reading.end = g_settings_get_string(settings, "japanese-reading-end");
  conf->jap_reading.font = g_settings_get_string(settings, "japanese-reading-font");
  gchar *jap_reading_color_str = g_settings_get_string(settings, "japanese-reading-color");
  conf->jap_reading.color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->jap_reading.color,
                 jap_reading_color_str);

  ////gloss
  conf->gloss.start = g_settings_get_string(settings, "gloss-start");
  conf->gloss.end = g_settings_get_string(settings, "gloss-end");
  conf->gloss.font = g_settings_get_string(settings, "gloss-font");
  gchar *gloss_color_str = g_settings_get_string(settings, "gloss-color");
  conf->gloss.color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->gloss.color, gloss_color_str);

  ////subgloss
  conf->subgloss.start = g_settings_get_string(settings, "subgloss-start");
  conf->subgloss.end = g_settings_get_string(settings, "subgloss-end");
  conf->subgloss.font = g_settings_get_string(settings, "subgloss-font");
  gchar *subgloss_color_str = g_settings_get_string(settings, "subgloss-color");
  conf->subgloss.color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->subgloss.color, subgloss_color_str);

  ////notes
  conf->notes.start = g_settings_get_string(settings, "notes-start");
  conf->notes.end = g_settings_get_string(settings, "notes-end");
  conf->notes.font = g_settings_get_string(settings, "notes-font");
  gchar *notes_color_str = g_settings_get_string(settings, "notes-color");
  conf->notes.color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->notes.color, notes_color_str);
  
  //highlight color
  conf->results_highlight_color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->results_highlight_color, str_results_highlight_color);
  
  //load the dictionaries
  GVariantIter iter;
  GVariant *dictionaries;
  dictionaries = g_settings_get_value(settings, "dictionaries");
  g_variant_iter_init(&iter, dictionaries);

  gchar *dicpath, *dicname;
  gboolean dicactive;

  while (g_variant_iter_next (&iter, "(&s&sb)", &dicpath, &dicname, &dicactive)) {
    if (dicpath != NULL) {
      WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
      dicfile->path = g_strdup(dicpath);
      dicfile->name = g_strdup(dicname);
      dicfile->is_active = dicactive;
      dicfile->is_loaded = FALSE;     //do not parse the dictionary yet

      //add the dictionary to the list
      conf->dicfile_list = g_slist_append(conf->dicfile_list, dicfile);
    }
  }
  g_variant_unref(dictionaries);

    
  //if (conf->dicfile_list != NULL) conf->selected_dic = conf->dicfile_list->data;
  
  //load the search options 
  conf->search_kata_on_hira = g_settings_get_boolean(settings, "search-kata-on-hira");
  conf->search_hira_on_kata = g_settings_get_boolean(settings, "search-hira-on-kata");
  conf->verb_deinflection = g_settings_get_boolean(settings, "deinflection-enabled");

  return conf;
}

void worddic_conf_save(GSettings *settings,
                       WorddicConfig *conf,
                       worddic_save fields){
  if(fields & WSE_HIGHLIGHT_COLOR){
    //save the result highlight color as a string
    char *str_results_highlight_color = gdk_rgba_to_string(conf->results_highlight_color);
    g_settings_set_string(settings, "results-highlight-color",
                          str_results_highlight_color);
  }

  if(fields & WSE_SEARCH_OPTION){
    g_settings_set_boolean(settings, "search-kata-on-hira",
                           conf->search_kata_on_hira);
    g_settings_set_boolean(settings, "search-hira-on-kata",
                           conf->search_hira_on_kata);
    g_settings_set_boolean(settings, "deinflection-enabled",
                           conf->verb_deinflection);
  }

  if(fields & WSE_DICFILE){
    GSList *diclist;
    WorddicDicfile *dicfile;

    //Save dicfiles [path and name seperated with linebreak]
    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("a(ssb)"));
    diclist = conf->dicfile_list;
    while (diclist != NULL) {
      if (diclist->data == NULL) break;
      dicfile = diclist->data;
      g_variant_builder_add(&builder,
                            "(ssb)",
                            dicfile->path,
                            dicfile->name,
                            dicfile->is_active);
      diclist = g_slist_next(diclist);
    }
    g_settings_set_value(settings, "dictionaries", g_variant_builder_end(&builder));

  }

  if(fields & WSE_JAPANESE_DEFINITION){
    g_settings_set_string(settings, "japanese-definition-start", conf->jap_def.start);
    g_settings_set_string(settings, "japanese-definition-end", conf->jap_def.end);
    g_settings_set_string(settings, "japanese-definition-font", conf->jap_def.font);
    char *str_jap_def_color = gdk_rgba_to_string(conf->jap_def.color);
    g_settings_set_string(settings, "japanese-definition-color", str_jap_def_color);

  }
  
  if(fields & WSE_JAPANESE_READING){
    g_settings_set_string(settings, "japanese-reading-start", conf->jap_reading.start);
    g_settings_set_string(settings, "japanese-reading-end", conf->jap_reading.end);
    g_settings_set_string(settings, "japanese-reading-font", conf->jap_reading.font);
    char *str_jap_reading_color = gdk_rgba_to_string(conf->jap_reading.color);
    g_settings_set_string(settings, "japanese-reading-color", str_jap_reading_color);

  }

  if(fields & WSE_GLOSS){
    g_settings_set_string(settings, "gloss-start", conf->gloss.start);
    g_settings_set_string(settings, "gloss-end", conf->gloss.end);
    g_settings_set_string(settings, "gloss-font", conf->gloss.font);
    char *str_gloss_color = gdk_rgba_to_string(conf->gloss.color);
    g_settings_set_string(settings, "gloss-color", str_gloss_color);

    g_settings_set_string(settings, "subgloss-start", conf->subgloss.start);
    g_settings_set_string(settings, "subgloss-end", conf->subgloss.end);
    g_settings_set_string(settings, "subgloss-font", conf->subgloss.font);
    char *str_subgloss_color = gdk_rgba_to_string(conf->subgloss.color);
    g_settings_set_string(settings, "subgloss-color", str_subgloss_color);

  }
  
  if(fields & WSE_NOTES){
    g_settings_set_string(settings, "notes-start", conf->notes.start);
    g_settings_set_string(settings, "notes-end", conf->notes.end);
    g_settings_set_string(settings, "notes-font", conf->notes.font);
    char *str_notes_color = gdk_rgba_to_string(conf->notes.color);
    g_settings_set_string(settings, "notes-color", str_notes_color);

  }
}
