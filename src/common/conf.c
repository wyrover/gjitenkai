#include "conf.h"

extern gchar *kanjidicstrg[];
extern gchar *gnome_dialog_msg;
GjitenConfig conf;
GSettings *settings, *kanjidic_settings;

GjitenConfig *conf_load() {
  gchar *tmpstrg, *tmpptr;
  GjitenDicfile *dicfile;
  GSList *diclist;
  GjitenConfig *conf;

  conf = g_new0(GjitenConfig, 1);

  //load the font familly for the results
  conf->resultsfont = g_settings_get_string(settings, "resultsfont");

  //load the results highlight color from string
  char *str_results_highlight_color = g_settings_get_string(settings, "results-highlight-color");

  //parse this color to RGBA object
  conf->results_highlight_color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->results_highlight_color, str_results_highlight_color);

  //check if dictionaries where loaded
  if (conf->dicfile_list != NULL) {
    dicutil_unload_dic();
    dicfile_list_free(conf->dicfile_list);
    conf->dicfile_list = NULL;
  }
  //load dictionaries
  {
    GVariantIter iter;
    GVariant *dictionaries;
    dictionaries = g_settings_get_value(settings, "dictionaries");
    g_variant_iter_init(&iter, dictionaries);
    diclist = NULL;
    while (g_variant_iter_next (&iter, "(&s&s)", &tmpstrg, &tmpptr)) {
      if (tmpstrg != NULL) {
        dicfile = g_new0(GjitenDicfile, 1);
        dicfile->path = g_strdup(tmpstrg);
        dicfile->name = g_strdup(tmpptr);
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

  //KANJIDIC options
  //new kanjidic file
  if (conf->kanjidic == NULL) conf->kanjidic = g_new0(GjitenDicfile, 1);
  conf->kanjidic->name = g_strdup("kanjidic");
  conf->kanjidic->path = g_settings_get_string(kanjidic_settings, "kanjidicfile");
  if ((conf->kanjidic->path == NULL) || (strlen(conf->kanjidic->path)) == 0) {
    conf->kanjidic->path = g_strdup(GJITENKAI_DICDIR"/kanjidic.utf8");
  }

  //kanji tag font and color
  conf->kanji_font = g_settings_get_string(kanjidic_settings, "kanji-font");

  //load the results highlight color from string
  char *str_kanji_color = g_settings_get_string(kanjidic_settings, "kanji-color");

  //parse this color to RGBA object
  conf->kanji_color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->kanji_color, str_kanji_color);

  return conf;
}

void conf_save(GjitenConfig *conf) {
  int i;
  GSList *diclist;
  GjitenDicfile *dicfile;
  
  g_settings_set_string(settings, "resultsfont", conf->resultsfont == NULL ? "" : conf->resultsfont);

  //save the result highlight color as a string
  char *str_results_highlight_color = gdk_rgba_to_string(conf->results_highlight_color);
  g_settings_set_string(settings, "results-highlight-color", str_results_highlight_color);
	
  g_settings_set_boolean(settings, "search-kata-on-hira", conf->search_kata_on_hira);
  g_settings_set_boolean(settings, "search-hira-on-kata", conf->search_hira_on_kata);
  g_settings_set_boolean(settings, "deinflection-enabled", conf->verb_deinflection);

  {
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
  }
  
  //Kanjidic options save
  g_settings_set_string(kanjidic_settings, "kanjidicfile", conf->kanjidic->path);
}

void conf_save_history(GList *history, GjitenConfig *conf) {
  GArray *array;
  int i;
  array = g_array_new(TRUE, TRUE, sizeof(gchar *));
  if (history != NULL) {
    for (i = 0; i <= 50; i++) {
      array = g_array_append_val(array, history->data);
      history = g_list_next(history);
      if (history == NULL) break;
    } 
    g_settings_set_strv(settings, "history", (const gchar **)array->data);
  }
  g_array_free(array, TRUE);
}

void conf_save_options(GjitenConfig *conf) {
  g_settings_set_boolean(settings, "autoadjust-enabled", conf->autoadjust_enabled);
  g_settings_set_boolean(settings, "searchlimit-enabled", conf->searchlimit_enabled);
  g_settings_set_uint(settings, "maxwordmatches", conf->maxwordmatches);
}

gboolean conf_init_handler() {
#if !GLIB_CHECK_VERSION(2, 32, 0)
  g_type_init();
#endif

  if (settings == NULL) {
    settings = g_settings_new(SETTINGS_WORDDIC);
    kanjidic_settings = g_settings_new("apps.gjitenkai.kanjidic");
  }

  return TRUE;

}

void conf_close_handler() {
  if (settings != NULL) {
    GJITEN_DEBUG("calling g_object_unref(G_OBJECT(settings)) [%d]\n", (int) settings);
    g_object_unref(G_OBJECT(settings));
    g_object_unref(G_OBJECT(kanjidic_settings));
    settings = NULL;
    kanjidic_settings = NULL;
  }
}

void dicutil_unload_dic() {
  if (conf.mmaped_dicfile != NULL) {
    //free mem of previously used dicfile	
    munmap(conf.mmaped_dicfile->mem, conf.mmaped_dicfile->size);
    conf.mmaped_dicfile->mem = NULL;
    conf.mmaped_dicfile = NULL;
  }
}
