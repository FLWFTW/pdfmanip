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

    ITERATOR Iter;
    AttachIterator( &Iter, pdf->trailer );
    pdfm_d *dic = NULL;
    while( ( dic = NextInList( &Iter ) ) != NULL )
    {
        switch( dic->type )
        {
            case PDFM_NUMBER:
                {
                    fprintf( stdout, "%s: %"PRIu64" (Number)\n", dic->label, (uint64_t)dic->content );
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
            default:
                {
                    break;
                }
        }
    }
    DetachIterator( &Iter );
    free_pdf( pdf );

    return 0;
}
