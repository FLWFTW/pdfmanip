#include "pdfmanip.h"

pdfm_d *new_dictionary()
{
   pdfm_d *d = calloc( 1, sizeof( pdfm_d ) );
   d->table = new_hash_table();
   d->list = AllocList();

   return d;
}

void free_dictionary( pdfm_d *d )
{
   free_hash_table( d->table );
   FreeList( d->list );
   free( d );
}

void dictionary_add( pdfm_dentry *entry, pdfm_d *d )
{
   hash_add( entry, entry->label, d->table );
   ITERATOR Iter;
   AttachIterator( &Iter, d->list );
   char *str; //See if this is a repeat/update of a hash table entry. If it is we don't need to
              //re-add the key to the key list.
   while( ( str = NextInList( &Iter ) ) != NULL )
   {
      if( !strcmp( str, entry->label ) )
         break;
   }
   if( !str ) //Key doesn't exist in key list, so we add it to the key list.
      AppendToList( entry->label, d->list );
   return;
}

pdfm_d *dictionary_get( char *key, pdfm_d *d )
{
   return (pdfm_d *)hash_get( key, d->table );
}

void dictionary_del( char *key, pdfm_d *d )
{
   pdfm_dentry *entry = hash_get( key, d->table );
   free( entry->label );
   free( entry );
   hash_del( key, d->table );
   ITERATOR Iter;
   char *str;
   AttachIterator( &Iter, d->list );
   while( ( str = NextInList( &Iter ) ) != NULL )
   {
      if( !strcmp( str, key ) )
         break;
   }
   DetachIterator( &Iter );
   DetachFromList( str, d->list );
}
