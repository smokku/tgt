#include "tgt_keyboard.h"
#include <stdio.h>
#include <stdlib.h>

/* Przydziela miejsce na lookup-table i zeruje ja, ustawiajac wszystkie
   klawisze na niepoprawne... Patrz include/tgt_keyboard.h */

struct tgt_keynode * tgt_allockeylookuptable(void)
{
    struct tgt_keynode * ret;
    int i;
    ret=(struct tgt_keynode*) malloc(sizeof(struct tgt_keynode)*256);
    for(i=0;i<256;i++) ret[i].type=TGT_KEYN_NONE;
    return(ret);
}

/* Inicjuje glowna lookup-table, w ktorej type(i)=TGT_KEYN_KEY a value(i)=i */

struct tgt_keynode * tgt_initroottable(void)
{
    int i;
    struct tgt_keynode *mt;
    mt=tgt_allockeylookuptable();
    for(i=0;i<256;i++)
    {
	mt[i].type=TGT_KEYN_KEY;
	mt[i].value=(void*) i;
    }
    return(mt);
}
/* Niszczy podtabele podanej lookup-table i nia sama przy okazji */

void tgt_destroylookuptable(struct tgt_keynode * lt)
{
    int i;
    
    for(i=0;i<256;i++)
	if(lt[i].type==TGT_KEYN_LOOKUPTABLE) tgt_destroylookuptable(lt[i].value);

//    fprintf(stderr,"ltfree\n");
    free(lt);
//    fprintf(stderr,"done\n");
}

/* Dopisuje sekwencje seq jako c do struktur key-lookup, ktorych najwyzsza
   lookup-table jest l_main_table */


void tgt_addkeyseq(struct tgt_keynode *l_main_table,char *seq,int c)
{
    int k,l;
    struct tgt_keynode *lt;
    struct tgt_keynode *nn;
    int ln,i;
    l=strlen(seq);
    lt=l_main_table;
    for(k=0;k<(l-1);k++)
    {
/* Bierzemy kolejny znak w sekwencji... */
	i=seq[k];
/* Sprawdzamy typ istniejacego dla niego wpisu.. */
	ln=lt[i].type;

	if(ln==TGT_KEYN_KEY || ln==TGT_KEYN_NONE)
	{
/* Brak wpisu lub istniejacy wpis jakies innej (krotszej) sekwencji (wtedy
   nadpisujemy stary wpis) : przydzielamy nowa lookup table i przypisujemy
   ja do wpisu naszego znaku w tej starej */
	    nn=tgt_allockeylookuptable();
	    lt[i].type=TGT_KEYN_LOOKUPTABLE;
	    lt[i].value=nn;
/* Tabela ta bedzie lookup-table dla nastepnego znaku, ktory rozpatrzymy w 
   nastepnym cyklu petli */
	    lt=nn;
	}
	else
/* Wpis lookup-table: kiedys zostala juz wpisana jakas sekwencja zblizona do naszej
   Wpis dla nastepnego znaku (nastepny cykl petli) bedzie sie znajdowal wlasnie 
   w tej tablicy */
	    lt=(struct tgt_keynode*) lt[i].value;
    }
/* Ostatni znak */
    i=seq[l-1];
/* Jesli w miejscu na ktore sie chcemy wpisac byla juz lookup-table to zwalniamy
   ja */
    if(lt[i].type==TGT_KEYN_LOOKUPTABLE) tgt_destroylookuptable(lt[i].value);
    lt[i].type=TGT_KEYN_KEY; lt[i].value=(void*) c;
}
