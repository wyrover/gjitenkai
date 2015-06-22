#include "conf.h"

WorddicConfig *worddic_conf_load(struct worddic_t *p_worddic){
  GSettings *settings = p_worddic->settings;
  gchar *tmpstrg, *tmpptr;
  GSList *diclist;
  WorddicConfig *conf;

  conf = g_new0(WorddicConfig, 1);

  //load the font familly for the results
  conf->resultsfont = g_settings_get_string(settings, "resultsfont");

  //load the results highlight color from string
  char *str_results_highlight_color = g_settings_get_string(settings, "results-highlight-color");

  //parse the color string to an RGBA object
  conf->results_highlight_color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->results_highlight_color, str_results_highlight_color);

  //check if dictionaries where loaded
  //if so, clear the memory
  if (conf->dicfile_list != NULL) {
    dicutil_unload_dic(conf->mmaped_dicfile);
    dicfile_list_free(conf->dicfile_list);
    conf->dicfile_list = NULL;
  }
  
  //load the dictionaries list
  {
    GVariantIter iter;
    GVariant *dictionaries;
    dictionaries = g_settings_get_value(settings, "dictionaries");
    g_variant_iter_init(&iter, dictionaries);
    diclist = NULL;
    while (g_variant_iter_next (&iter, "(&s&s)", &tmpstrg, &tmpptr)) {
      if (tmpstrg != NULL) {

        //create the worddic dictionary
        WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
        dicfile->path = g_strdup(tmpstrg);
        dicfile->name = g_strdup(tmpptr);

        //load the dictionary content into memory
        dicfile_load((GjitenDicfile*)dicfile, NULL);
        dicfile->entries = NULL;
        dicfile->entries =  worddic_dicfile_parse(dicfile);

        //add the dictionary to the list
        conf->dicfile_list = g_slist_append(conf->dicfile_list, dicfile);
      }
    }
    g_variant_unref(dictionaries);
  }
  if (conf->dicfile_list != NULL) conf->selected_dic = conf->dicfile_list->data;
  
  //load the search options 
  conf->search_kata_on_hira = g_settings_get_boolean(settings, "search-kata-on-hira");
  conf->search_hira_on_kata = g_settings_get_boolean(settings, "search-hira-on-kata");
  conf->verb_deinflection = g_settings_get_boolean(settings, "deinflection-enabled");

  return conf;
}

void worddic_conf_save(struct worddic_t *p_worddic){
  int i;
  GSList *diclist;
  GjitenDicfile *dicfile;
  GSettings *settings = p_worddic->settings;
  WorddicConfig *conf = p_worddic->conf;
  
  g_settings_set_string(settings, "resultsfont",
                        conf->resultsfont == NULL ? "" : conf->resultsfont);

  //save the result highlight color as a string
  char *str_results_highlight_color = gdk_rgba_to_string(conf->results_highlight_color);
  g_settings_set_string(settings, "results-highlight-color", str_results_highlight_color);
	
  g_settings_set_boolean(settings, "search-kata-on-hira", conf->search_kata_on_hira);
  g_settings_set_boolean(settings, "search-hira-on-kata", conf->search_hira_on_kata);
  g_settings_set_boolean(settings, "deinflection-enabled", conf->verb_deinflection);

  //Save dicfiles [path and name seperated with linebreak]
  GVariantBuilder builder;
  g_variant_builder_init(&builder, G_VARIANT_TYPE("a(ss)"));
  diclist = conf->dicfile_list;
  while (diclist != NULL) {
    if (diclist->data == NULL) break;
    dicfile = diclist->data;
    g_variant_builder_add(&builder, "(ss)", dicfile->path, dicfile->name);
    diclist = g_slist_next(diclist);
  }
  g_settings_set_value(settings, "dictionaries", g_variant_builder_end(&builder));

  g_settings_set_boolean(settings, "autoadjust-enabled", conf->autoadjust_enabled);
  g_settings_set_boolean(settings, "searchlimit-enabled", conf->searchlimit_enabled);
  g_settings_set_uint(settings, "maxwordmatches", conf->maxwordmatches);

}
