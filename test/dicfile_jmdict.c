//gcc -g  dicfile_jmdict.c print_entry.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0 libxml-2.0) -lz -I../src/worddic/ -o dicfile_jmdict
#include <gtk/gtk.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "print_entry.h"
#include "../src/worddic/worddic_dicfile.h"
#include "../src/worddic/dicentry.h"
#include "../src/worddic/gloss.h"

WorddicDicfile *dicfile;

/**
   return a GjitenDicEntry from an entry in a JMdict
   cur must point to an entry
 */
GjitenDicentry* parse_entry_jmdict(xmlNodePtr cur){
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);
  gloss *p_gloss = g_new0(gloss, 1);
  dicentry->gloss = g_slist_prepend(dicentry->gloss, p_gloss);
  xmlChar *key;
  cur = cur->xmlChildrenNode;
  while (cur) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"sense"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"gloss"))){
	  sub_gloss *p_sub_gloss = g_new0(sub_gloss, 1);
	  p_gloss->sub_gloss = g_slist_prepend(p_gloss->sub_gloss, p_sub_gloss);

	  gchar *content = xmlNodeGetContent(child);
	  p_sub_gloss->content = content;
	  g_printf("Content is %s\n", p_sub_gloss->content);

	  gchar *lang = xmlGetProp(child, "lang");
	  if(lang){
	    strncpy(p_sub_gloss->lang, lang, 3);
	    xmlFree(lang);
	  }
	}
	else if((!xmlStrcmp(child->name, (const xmlChar *)"pos")) ||
		(!xmlStrcmp(child->name, (const xmlChar *)"misc"))){
	  gchar *content = xmlNodeGetContent(child);
	  p_gloss->general_informations = g_slist_prepend(p_gloss->general_informations, content);
	}


	child = child->next;
      }
    }
    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"k_ele"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"keb"))){
	  gchar *content = xmlNodeGetContent(child);
	  dicentry->jap_reading = g_slist_prepend(dicentry->jap_reading, content);
	}
	child = child->next;
      }
    }
    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"r_ele"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"reb"))){
	  gchar *content = xmlNodeGetContent(child);
	  dicentry->jap_definition = g_slist_prepend(dicentry->jap_definition, content);
	}
	child = child->next;
      }
    }

    cur = cur->next;
  }
  return dicentry;
}

void dicfile_parse_jmdict(char *filepath){
  xmlDocPtr doc;
  xmlNodePtr cur;
  doc = xmlParseFile(filepath);

  if (doc == NULL ) {
    fprintf(stderr,"Document not parsed successfully. \n");
    return;
  }
  cur = xmlDocGetRootElement(doc);

  if (xmlStrcmp(cur->name, (const xmlChar *) "JMdict")) {
    fprintf(stderr,"document of the wrong type, root node != JMdict");
    xmlFreeDoc(doc);
    return;
  }

  cur = cur->xmlChildrenNode;
  while (cur != NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"entry"))){
      GjitenDicentry *dicentry = parse_entry_jmdict(cur);
      dicfile->entries = g_slist_prepend(dicfile->entries, dicentry);
    }

    cur = cur->next;
  }

  xmlFreeDoc(doc);
}

int main( int argc, char **argv ){
  g_printf("load a jmdict dictionary XML file.\n parameters are:\n\
'Dicionary path'\n\
'whatever second argument to print all entries'\n");

  char *path = argv[1];
  int print_all = FALSE;
  if(argc > 2)print_all = TRUE;

  dicfile = g_new0(WorddicDicfile, 1);
  dicfile_parse_jmdict(path);

  /*WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(path);
  worddic_dicfile_open(dicfile);
  worddic_dicfile_parse_all(dicfile);*/

  if(print_all){
    //print all entries
    GSList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
	list_dicentry != NULL;
	list_dicentry = list_dicentry->next){
      print_entry(list_dicentry->data);
    }
  }

  //worddic_dicfile_close(dicfile);
  //worddic_dicfile_free(dicfile);
  return 1;
}
