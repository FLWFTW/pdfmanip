#include <stdio.h>
#include "pdfmanip.h"

int main( int numArgs, char **argList )
{
    
    pdfm *pdf = load_pdf( "test.pdf" );
    if( pdf == NULL )
    {
       fprintf( stdout, "Error loading pdf file test.pdf. %s\n",  pdfm_error_message[pdfm_error_code] );
       return 1;
    }
    fprintf( stdout, "Filename: %s\nSize: %zu\nVersion: %s\nXREF Table at: %zu\n", pdf->filename, pdf->size, pdf->version_string, pdf->xref_location );

    pdfm_o *o;
    ITERATOR Iter;
    fprintf( stdout, "\nPDF Object Listing:\n" );
    AttachIterator( &Iter, pdf->objects );
    while( ( o = NextInList( &Iter ) ) != NULL )
    {
       fprintf( stdout, "Object: %"PRIu64" %"PRIu64"\nOffset: %zu\nState: %c\n\n", o->object_number, o->generation_number, o->offset, o->status );
    }
    DetachIterator( &Iter );

    free_pdf( pdf );

    return 0;
}
