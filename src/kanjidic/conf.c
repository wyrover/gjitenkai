#include "conf.h"

KanjidicConfig *kanjidic_conf_load(kanjidic *p_kanjidic) {
  GSettings *kanjidic_settings = p_kanjidic->settings;
  KanjidicConfig * conf = g_new0(KanjidicConfig, 1);

  //new kanjidic file
  if (conf->kanjidic == NULL) conf->kanjidic = g_new0(GjitenDicfile, 1);
  conf->kanjidic->name = g_strdup("kanjidic");
  conf->kanjidic->path = g_settings_get_string(kanjidic_settings, "kanjidicfile");

  if ((conf->kanjidic->path == NULL) || (strlen(conf->kanjidic->path)) == 0) {
    const gchar * const * dirs = g_get_system_data_dirs();
    gchar *rest = g_strjoin(G_DIR_SEPARATOR_S, PROJECT_NAME, "kanjidic.utf8", NULL);
    gchar* filename = get_file(dirs, rest);
    g_free(rest);
    conf->kanjidic->path = filename;
  }

  //kanji tag font and color
  conf->kanji_font = g_settings_get_string(kanjidic_settings, "kanji-font");

  //load the results highlight color from string
  char *str_kanji_color = g_settings_get_string(kanjidic_settings,
                                                "kanji-color");

  //parse this color to RGBA object
  conf->kanji_color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->kanji_color, str_kanji_color);

  conf->separator = g_settings_get_string(kanjidic_settings, "separator");


  //kanji result font and color
  //kanji tag font and color
  conf->kanji_result_font = g_settings_get_string(kanjidic_settings,
                                                  "kanji-result-font");
  return conf;
}

void kanjidic_conf_save(KanjidicConfig *conf, GSettings *settings) {

  //Kanjidic options save
  //path of the kanjidic
  g_settings_set_string(settings, "kanjidicfile", conf->kanjidic->path);

  //kanji font and color
  g_settings_set_string(settings, "kanji-font", conf->kanji_font);
  char *str_kanji_color = gdk_rgba_to_string(conf->kanji_color);
  g_settings_set_string(settings, "kanji-color", str_kanji_color);

  g_settings_set_string(settings, "separator", conf->separator);

  //kanji result button font
  g_settings_set_string(settings, "kanji-result-font", conf->kanji_result_font);

}
