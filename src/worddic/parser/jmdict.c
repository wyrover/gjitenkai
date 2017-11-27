#include "jmdict.h"

GjitenDicentry* parse_entry_jmdict(xmlDocPtr doc, xmlNodePtr cur){
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);
  sense *p_sense = g_new0(sense, 1);
  dicentry->sense = g_slist_prepend(dicentry->sense, p_sense);

  cur = cur->xmlChildrenNode;

  while (cur) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"sense"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"gloss"))){
	  sub_sense *p_sub_sense = g_new0(sub_sense, 1);
	  p_sense->sub_sense = g_slist_prepend(p_sense->sub_sense, p_sub_sense);

	  gchar *content = (gchar *)xmlNodeGetContent(child);
	  p_sub_sense->content = content;

	  gchar *lang = (gchar *)xmlGetProp(child, (const xmlChar *)"lang");
	  if(lang){
	    strncpy(p_sub_sense->lang, lang, 3);
	    xmlFree(lang);
	  }
	}
	else if((!xmlStrcmp(child->name, (const xmlChar *)"pos"))
		// || (!xmlStrcmp(child->name, (const xmlChar *)"misc"))
		){
	  //get the content without entity subtitution
	  gchar *content = xmlNodeListGetRawString(doc, child->xmlChildrenNode, 0);
	  p_sense->general_informations = g_slist_prepend(p_sense->general_informations, content);
	}
	child = child->next;
      }
    }
    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"k_ele"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"keb"))){
	  gchar *content = (gchar *)xmlNodeGetContent(child);
	  dicentry->jap_reading = g_slist_prepend(dicentry->jap_reading, content);
	}
	child = child->next;
      }
    }
    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"r_ele"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"reb"))){
	  gchar *content = (gchar *)xmlNodeGetContent(child);
	  dicentry->jap_definition = g_slist_prepend(dicentry->jap_definition, content);
	}
	child = child->next;
      }
    }

    cur = cur->next;
  }

  //dicentry->GI = GIALL;   //TODO_GI moved to sense
  //dicentry_set_GI_flags_from_code(dicentry);

  return dicentry;
}