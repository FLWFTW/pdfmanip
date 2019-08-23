#include "pdfmanip.h"

static int pdfft_getsize( pdfft *pdf )
{
   size_t count = 0;
   fpos_t start;
   fgetpos( pdf->fp, &start );
   do
   {
      fgetc( pdf->fp );
      count++;
   }while( !feof( pdf->fp ) );
   fsetpos( pdf->fp, &start );

   return count -1;
}

static void pdfft_loadversion( pdfft *pdf )
{
    char *pch = memchr( pdf->raw_data, '\n', pdf->size );
    pdf->version_string = calloc( 1, sizeof(char) * ( ( pch - pdf->raw_data) + 1 ) ); //allocate enough memory to hold the version string (first line) of the pdf file
    memcpy( pdf->version_string, pdf->raw_data, ( pch - pdf->raw_data ) );
}

static char *pdfft_parsestring( char *pch )
{
   return NULL;
}

static void pdfft_parsedictionary( char *pch )
{
}

static char *pdfft_parsehexstring( char *pch )
{
   return NULL;
}

static void pdfft_parsearray( char *pch )
{
}

static void pdfft_parsename( char *pch )
{
}


static char *pdfft_parsecomment( char *pch )
{
   while( *pch != WSP_LF && *pch != WSP_CR )
      pch++;

   return pch;
}

static int process_raw_pdf_data( pdfft *pdf )
{
   char *pch = pdf->raw_data;

   if( pch[0] != '%' || pch[1] != 'P' || pch[2] != 'D' || pch[3] != 'F' )
      return 1;

   while( strcmp( pch, "%%EOF" ) )
   {
      switch( *pch )
      {
         case DLM_LP:
            {
               pdfft_parsestring( pch );
               break;
            }
         case DLM_LT:
            {
               if( *pch + 1 == DLM_LT )
               {
                  pdfft_parsedictionary( pch );
               }
               else
               {
                  pdfft_parsehexstring( pch );
               }
               break;
            }
         case DLM_LB:
            {
               pdfft_parsearray( pch );
               break;
            }
         case DLM_LC:
            {
               break;
            }
         case DLM_SD:
            {
               pdfft_parsename( pch );
               break;
            }
         case DLM_PC:
            {
               pdfft_parsecomment( pch );
               break;
            }
         default:
            {
               break;
            }
      }
   }
   

   return 0;
}

pdfft *new_pdf()
{
   pdfft *pdf = calloc( 1, sizeof( pdfft ) );
   if( pdf == NULL )
      return NULL;
   pdf->objects = AllocList();

   return pdf;
}

void free_pdf( pdfft *pdf )
{
   free( pdf->version_string );
   free( pdf->raw_data );
   free( pdf->filename );
   ITERATOR Iter;
   AttachIterator( &Iter, pdf->objects );
   pdfft_io *io;
   while( ( io = NextInList( &Iter ) ) != NULL )
   {
      free( io );
   }
   FreeList( pdf->objects );
}


pdfft *load_pdf( char *filename )
{
    pdfft *pdf = new_pdf();
 
    pdf->filename = strdup( filename );
    pdf->fp = fopen( pdf->filename, "r" );
    pdf->size = pdfft_getsize( pdf );
    pdf->raw_data = calloc( 1, sizeof(char) * pdf->size );
    size_t size = 0;
    if( pdf->size != ( size = fread( pdf->raw_data, sizeof(char), pdf->size, pdf->fp ) ) )
    {
       fprintf( stderr, "Error reading file %s. Expected %zu bytes. Read %zu bytes. Aborting.\n", pdf->filename, pdf->size, size );
       return NULL;
    }

    pdfft_loadversion( pdf );
    if( process_raw_pdf_data( pdf ) != 0 )
    {
       free_pdf( pdf );
       return NULL;
    }

    return pdf;

}
