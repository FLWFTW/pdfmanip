#ifndef __pdfmanip_dictionary_h__
#define __pdfmanip_dictionary_h__

#include "pdfmanip.h"

pdfm_d *new_dictionary();
void free_dictionary( pdfm_d *d );

void dictionary_add( pdfm_dentry *entry, pdfm_d *d );
pdfm_d *dictionary_get( char *key, pdfm_d *d );
void dictionary_del( char *key, pdfm_d *d );
#endif
