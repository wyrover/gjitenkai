#include <locale.h>
#include <libintl.h>

#include "gjitenkai.h"
#include "../gjitenkai/dicutil.h"

extern gboolean on_gjitenkai_search_results_button_release_event(GtkWidget *text_view,
                                                                 GdkEventButton *event,
                                                                 gjitenkai *gjitenkai);
extern void on_search_expression_activate(GtkEntry *entry,
                                          worddic *worddic);

extern void on_gjitenkai_search_expression_activate(GtkEntry *entry,
                                                     gjitenkai *gjitenkai);

extern void on_button_download_clicked(GtkButton* button, worddic *p_worddic);


GError *error = 0;

int main( int argc, char **argv){
  const gchar * const * dirs = g_get_system_data_dirs();
  gchar *rest = g_strjoin(G_DIR_SEPARATOR_S, "locale", NULL);
  gchar* localdir = get_file(dirs, rest);
  g_free(rest);

  if(localdir){
    bindtextdomain("gjitenkai", localdir);
    textdomain("gjitenkai");
    bindtextdomain("worddic", localdir);
    textdomain("worddic");
    bindtextdomain("kanjidic", localdir);
    textdomain("kanjidic");
  }

  g_free(localdir);

  gjitenkai gjitenkai;
  gjitenkai.worddic = g_new0(worddic, 1);
  gjitenkai.kanjidic = g_new0(kanjidic, 1);

  //init gtk
  gtk_init (&argc, &argv);

  session = NULL;

  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;

  //init application
  gjitenkai_init (&gjitenkai);
  worddic_init(gjitenkai.worddic);
  kanjidic_init(gjitenkai.kanjidic);

  //add worddic history in the history menu
  GSList *l=NULL;
  for(l=gjitenkai.worddic->conf->history;l!=NULL;l = l->next){
    gchar *searched_expression = (gchar*)l->data;
    gjitenkai_menu_history_append(&gjitenkai, searched_expression);
  }

  GtkWindow *window = (GtkWindow*)gtk_builder_get_object(gjitenkai.definitions,
                                                         "gjiten");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);

  //get the top level box of worddic and kanjidic
  GtkWidget *box_worddic = (GtkWidget*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                              "box_toplevel");
  GtkWidget *box_kanjidic = (GtkWidget*)gtk_builder_get_object(gjitenkai.kanjidic->definitions,
                                                               "box_toplevel");

  //create the paned widget and the notebook
  gjitenkai.paned = (GtkPaned*)gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gjitenkai.notebook = (GtkNotebook*)gtk_notebook_new();

  //by default, use the paned widget
  gtk_paned_pack1(gjitenkai.paned, box_worddic, TRUE, FALSE);
  gtk_paned_pack2(gjitenkai.paned, box_kanjidic, TRUE, FALSE);
  gtk_paned_set_position(gjitenkai.paned, 400);

  //create the page where to append the worddic and kanjidic boxes if the
  //selected view changes to notebook
  gtk_notebook_append_page_menu (gjitenkai.notebook,
				 gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0),
                                 gtk_label_new("Worddic"),
                                 NULL);
  gtk_notebook_append_page_menu (gjitenkai.notebook,
                                 gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0),
                                 gtk_label_new("Kanjidic"),
                                 NULL);

  //append the main container to the gjiten box_top
  GtkWindow *box_top = (GtkWindow*)gtk_builder_get_object(gjitenkai.definitions,
                                                          "box_top");

  gtk_box_pack_start(GTK_BOX(box_top), GTK_WIDGET(gjitenkai.paned), TRUE, TRUE, 0);

  //Construct the pref dialog
  //pref content box at toplevel for worddic and kanjidic
  GtkWidget *worddic_box_toplevel_prefs = (GtkWidget*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                                             "box_toplevel_prefs");
  GtkWidget *kanjidic_box_toplevel_prefs = (GtkWidget*)gtk_builder_get_object(gjitenkai.kanjidic->definitions,
                                                                              "box_toplevel_prefs");
  //append the worddic and kanjidic preferences boxes in the gjitenkai
  //applications notebook
  GtkWidget *notebook_apps = (GtkWidget*)gtk_builder_get_object(gjitenkai.definitions,
                                                                "notebook_apps");
  gtk_notebook_append_page_menu (GTK_NOTEBOOK(notebook_apps),
                                 worddic_box_toplevel_prefs,
                                 gtk_label_new("Worddic"),
                                 NULL);
  gtk_notebook_append_page_menu (GTK_NOTEBOOK(notebook_apps),
                                 kanjidic_box_toplevel_prefs,
                                 gtk_label_new("Kanjidic"),
                                 NULL);


  //append the search menuitems of worddic to the gjitenkai main menubar
  //main menubar from gjitenkai
  GtkMenuBar* main_menu_bar = (GtkMenuBar*)gtk_builder_get_object(gjitenkai.definitions,
                                                                  "menubar");

  //worddic search menu item
  GtkMenuItem* menu_item_search = (GtkMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                                       "menuitem_search");

  //remove the search item from the worddic menubar
  GtkMenuBar* worddic_menubar = (GtkMenuBar*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                                    "menubar");
  g_object_ref(menu_item_search);
  gtk_container_remove (GTK_CONTAINER(worddic_menubar), GTK_WIDGET(menu_item_search));

  //add the search item to the gjitenkai menubar
  gtk_menu_shell_insert(GTK_MENU_SHELL(main_menu_bar),
                        GTK_WIDGET(menu_item_search),
                        3);

  //recreate the accelerators
  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  GtkRadioMenuItem* radio_item = NULL;

  //jap exact
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                         "menuitem_search_japanese_exact");

  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group,
  'x', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  //jap any
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                         "menuitem_search_japanese_any");

  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group,
  'y', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  //jap start
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                         "menuitem_search_japanese_start");

  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group,
  's', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  //jap end
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                         "menuitem_search_japanese_end");

  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group,
  'e', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  //latin whole expression
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                         "menuitem_search_latin_whole_expressions");

  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group,
  'e', GDK_CONTROL_MASK | GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  //latin whole words
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                         "menuitem_search_latin_whole_words");

  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group,
  'w', GDK_CONTROL_MASK | GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  //latin any
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                         "menuitem_search_latin_any");

  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group,
  'y', GDK_CONTROL_MASK | GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  //////
  //Callbacks that needs connunication between worddic and kanjidic
  //callback when the worrdic search result is clicked, get the clicked
  //character, if it's a kanji, display it in kanjidic
  GtkTextView *result_text_view = (GtkTextView *)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                                        "search_results");
  g_signal_connect(result_text_view,
                   "button-release-event",
                   G_CALLBACK(on_gjitenkai_search_results_button_release_event),
                   &gjitenkai);

  //callback to expend radical list between fullwidth <bracets> before search.
  GtkWidget *entry = (GtkWidget*)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                        "search_expression");
  //disonnect the worddic search callback
  g_signal_handlers_disconnect_by_func(entry,
                                       G_CALLBACK(on_search_expression_activate),
                                       gjitenkai.worddic);
  //connect to search gjitenkai callback function
  g_signal_connect(entry,
                   "activate",
                   G_CALLBACK(on_gjitenkai_search_expression_activate),
                   &gjitenkai);



  //search css
  GError *error = 0;
  rest = "gjitenkai/style.css";
  gchar *filename_css = get_file(dirs, rest);
  if(filename_css){
    provider = gtk_css_provider_new();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen,
					       GTK_STYLE_PROVIDER (provider),
					       GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_file(provider, g_file_new_for_path(filename_css), &error);
  }

  // Initialisation of dictionary available to downlaod url
  rest = g_strjoin(G_DIR_SEPARATOR_S,
		   PROJECT_NAME,
		   "url",
		   NULL);
  gchar *filename = get_file(dirs, rest);
  g_free(rest);

  if(filename){
    FILE *fp = fopen(filename, "r");
    gchar desc[1024];
    gchar url[1024];
    int count;

    GtkBox *box_download = (GtkBox*)gtk_builder_get_object(gjitenkai.worddic->definitions, "box_download");

    while(!feof(fp)){
      count = fscanf(fp, "\"%[^\"]\" %s", desc, url);
      fgetc(fp);
      if(count == 2){
	GtkProgressBar *pbar = gtk_progress_bar_new();

	GtkWidget *button_download = gtk_button_new_with_label("Download");
	gchar *data = g_strdup(url);       //TODO free
	g_object_set_data(button_download, "url", data);
	g_signal_connect(button_download,
			 "clicked",
			 G_CALLBACK(on_button_download_clicked),
			 pbar);

	//contains the download button and the progressbar
	GtkBox *box_btn_progess = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(box_btn_progess, pbar, FALSE, TRUE, 0);
	gtk_box_pack_start(box_btn_progess, button_download, FALSE, TRUE, 0);

	GtkWidget *label_desc = gtk_label_new(desc);

	GtkBox *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(box, label_desc, TRUE, TRUE, 0);
	gtk_box_pack_start(box, box_btn_progess, FALSE, TRUE, 0);
	gtk_box_pack_start(box_download, box, TRUE, TRUE, 0);
      }
    }

    fclose(fp);
  }

  //country code and name
  rest = g_strjoin(G_DIR_SEPARATOR_S,
		   PROJECT_NAME,
		   "country_codes",
		   NULL);
  gchar *filename_country_codes = get_file(dirs, rest);
  g_free(rest);

  if(filename_country_codes){
    FILE *fp = fopen(filename_country_codes, "r");
    gchar country_name[256];
    gchar country_code[3];
    int count;

    GtkBox *box_lang = (GtkBox*)gtk_builder_get_object(gjitenkai.worddic->definitions, "box_lang");

    while(!feof(fp)){
      count = fscanf(fp, "%s %s", country_name, country_code);
      fgetc(fp);
      if(count == 2){
	GtkCheckButton *cb_lang = gtk_check_button_new_with_label (country_name);
	gchar *data = g_strdup(country_code);       //TODO free
	g_object_set_data(cb_lang, "lang", data);
	//g_signal_connect(cb_lang, "clicked", G_CALLBACK(on_cb_lang_toggle), gjitenkai.worddic);

	gtk_box_pack_start(box_lang, cb_lang, TRUE, TRUE, 0);
      }
    }

    fclose(fp);
  }


  //show and main loop
  gtk_widget_show_all ((GtkWidget*)window);
  gtk_main ();

  //save
  worddic_conf_save(gjitenkai.worddic->settings,
                    gjitenkai.worddic->conf,
                    WSE_HISTORY);

  return 1;
}
