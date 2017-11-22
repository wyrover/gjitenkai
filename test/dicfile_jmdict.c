//gcc -g  dicfile_jmdict.c print_entry.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0 libxml-2.0) -lz -I../src/worddic/ -o dicfile_jmdict
#include <gtk/gtk.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "print_entry.h"
#include "../src/worddic/worddic_dicfile.h"
#include "../src/worddic/dicentry.h"

WorddicDicfile *dicfile;

/**
   return a GjitenDicEntry from an entry in a JMdict
   cur must point to an entry
 */
GjitenDicentry* parse_entry_jmdict(xmlDocPtr doc, xmlNodePtr cur){
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);
  xmlChar *key;
  cur = cur->xmlChildrenNode;
  while (cur != NULL) {
    g_printf("> %s\n", cur->name);
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"sense"))){
      xmlNodePtr ccur = cur->xmlChildrenNode;
      while (ccur != NULL){
	g_printf(">> %s\n", (const xmlChar *)ccur->name);

	if((!xmlStrcmp(ccur->name, (const xmlChar *)"gloss"))){
	  g_printf(">>> %s\n", xmlNodeGetContent(cur));
	}

	ccur = ccur->next;
      }

      //g_printf(">> %s\n", xmlNodeGetContent(cur));
      //dicentry->gloss = g_slist_prepend(dicentry->gloss, p_gloss);

      //key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      //printf("keyword: %s\n", key);
      //xmlFree(key);
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
      parse_entry_jmdict(doc, cur);
    }

    cur = cur->next;
  }

}

int main( int argc, char **argv ){
  g_printf("load a jmdict dictionary XML file.\n parameters are:\n\
'Dicionary path'\n						  \
'whatever second argument to print all entries'\n");

  dicfile = g_new0(WorddicDicfile, 1);

  char *path = argv[1];
  int print_all = FALSE;
  if(argc > 2)print_all = TRUE;

  dicfile_parse_jmdict(path);

  /*  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(path);
  worddic_dicfile_open(dicfile);
  worddic_dicfile_parse_all(dicfile);

  if(print_all){
    //print all entries
    GSList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
	list_dicentry != NULL;
	list_dicentry = list_dicentry->next){
      print_entry(list_dicentry->data);
    }
  }

  worddic_dicfile_close(dicfile);
  worddic_dicfile_free(dicfile);*/
  return 1;
}
