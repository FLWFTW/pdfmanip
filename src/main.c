#include <stdio.h>
#include "pdfmanip.h"
#include "pdfmanip_dictionary.h"

int main( int numArgs, char **argList )
{
   hash_table *table = new_hash_table( 1000 );
   pdfm *pdf = NULL;
   if( numArgs > 1 )
   {
      pdf = load_pdf( argList[1] );
   }
   else
   {
      pdf = load_pdf( "test.pdf" );
   }
   if( pdf == NULL )
   {
     fprintf( stdout, "Error loading pdf file %s. %s\n",  argList[1], pdfm_error_message[pdfm_error_code] );
     return 1;
   }
   hash_add( pdf, "pdf", table );
   pdf = hash_get( "pdf", table );
   fprintf( stdout, "Filename: %s\nSize: %zu\nVersion: %s\nXREF Table at: %zu\n", pdf->filename, pdf->size, pdf->version_string, pdf->xref_location );

   ITERATOR Iter;
   AttachIterator( &Iter, pdf->trailer->list );
   pdfm_dentry *dic = NULL;
   char *key;
   while( ( key = NextInList( &Iter ) ) != NULL )
   {
       dic = (pdfm_dentry *)dictionary_get( key, pdf->trailer );
       switch( dic->type )
       {
          case PDFM_NUMBER:
             {
                fprintf( stdout, "%s: %"PRIu64" (Number)\n", dic->label, *(uint64_t*)dic->content );
                break;
             }
          case PDFM_IR:
             {
                fprintf( stdout, "%s: %s (Indirect Reference)\n", dic->label, (char*)dic->content );
                break;
             }
          case PDFM_NAME:
             {
                fprintf( stdout, "%s: %s (Name)\n", dic->label, (char*)dic->content );
                break;
             }
          case PDFM_ARRAY:
             {
                fprintf( stdout, "%s: %s (Array)\n", dic->label, (char*)dic->content );
                break;
             }
          default:
             {
                break;
             }
       }
   }
   DetachIterator( &Iter );

   AttachIterator( &Iter, pdf->objects );
   pdfm_o *obj = NULL;
   while( ( obj = NextInList( &Iter ) ) != NULL )
   {
      fprintf( stdout, "Offset: %"PRIu64"\nLength: %"PRIu64"\nType: %s\n", obj->offset, obj->size, object_type_names[obj->type] );
   }
   DetachIterator( &Iter );
   free_pdf( pdf );

   return 0;
}
