#include <stdio.h>
#include "pdfmanip.h"

int main( int numArgs, char **argList )
{
    
    pdfft *pdf = load_pdf( "test.pdf" );
    if( pdf == NULL )
    {
       fprintf( stdout, "Error loading pdf file test.pdf.\n" );
       return 1;
    }
    fprintf( stdout, "Filename: %s\nSize: %zu\nVersion: %s\nObjects: %d\n", pdf->filename, pdf->size, pdf->version_string, SizeOfList( pdf->objects ) );
    ITERATOR Iter;
    AttachIterator( &Iter, pdf->objects );
    pdfft_io *pObj;
    while( ( pObj = NextInList( &Iter ) ) != NULL )
    {
       fprintf( stdout, "Object:\n\tObject Number: %ud\n\tGeneration Number: %ud\n", pObj->object_number, pObj->generation_number );
    }
    DetachIterator( &Iter );

    free_pdf( pdf );

    return 0;
}
