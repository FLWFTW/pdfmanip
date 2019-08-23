#include "pdfmanip.h"

static pdfm_o *new_pdfmo();

static int pdfm_getsize( pdfm *pdf )
{
   uint64_t count = 0;
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

static void pdfm_loadversion( pdfm *pdf )
{
    char *pch = memchr( pdf->raw_data, '\n', pdf->size );
    pdf->version_string = calloc( 1, sizeof(char) * ( ( pch - pdf->raw_data) + 1 ) ); /*allocate enough memory to hold the version string (first line) of the pdf file*/
    memcpy( pdf->version_string, pdf->raw_data, ( pch - pdf->raw_data ) );
}

/*
static char *next_space( char *str )
{
   while( *str != '\0' && !isspace( *str ) )
      str++;
   return str;
}
*/


static int process_raw_pdf_data( pdfm *pdf )
{
   char *pch = pdf->raw_data;
   char *pos = pch + pdf->size;

   if( pch[0] != '%' || pch[1] != 'P' || pch[2] != 'D' || pch[3] != 'F' ) //No pdf version number
      return PDFM_NOVERSION;
   if( strcmp( pch+(pdf->size-7), "\n%%EOF\n" ) ) //No EOF marker
      return PDFM_NOEOF;

   while( strncmp( pos, "startxref", 9 ) ) //find the reference location of the xref table
      pos--;

   pos += 9;
   pdf->xref_location = strtoull( pos, NULL, 0 );
   pos = pch + pdf->xref_location;

   if( strncmp( pos, "xref", 4 ) ) //xref table not found
      return PDFM_NOXREF;

   pos += 5;
   strtoul( pos, &pos, 0 );
   pdf->xref_count = strtoull( pos, &pos, 0 );

   pdfm_o *o;
   int i;
   for( i = 0; i < pdf->xref_count; i++ )
   {
      o = new_pdfmo();
      o->offset = strtouint64_t( pos );
      pos += 11;
      o->generation_number = strtouint64_t( pos );
      pos += 7;
      o->status = *pos;
      pos += 2;
      AppendToList( o, pdf->objects );
   }

   pos++;
   if( strncmp( pos, "trailer", 7 ) )
   {
      fprintf( stderr, "%s", pos );
      return PDFM_NOTRAILER;
   }


   return 0;
}

pdfm *new_pdf()
{
   pdfm *pdf = calloc( 1, sizeof( pdfm ) );
   if( pdf == NULL )
      return NULL;
   pdf->objects = AllocList();

   return pdf;
}

static pdfm_o *new_pdfmo()
{
   pdfm_o *o = calloc( 1, sizeof( pdfm_o ) );
   o->contents = AllocList();

   return o;
}

void free_pdf( pdfm *pdf )
{
   free( pdf->version_string );
   free( pdf->raw_data );
   free( pdf->filename );
   ITERATOR Iter;
   AttachIterator( &Iter, pdf->objects );
   pdfm_o *io;
   while( ( io = NextInList( &Iter ) ) != NULL )
   {
      free( io );
   }
   FreeList( pdf->objects );
}


pdfm *load_pdf( char *filename )
{
    pdfm *pdf = new_pdf();
 
    pdf->filename = strdup( filename );
    pdf->fp = fopen( pdf->filename, "r" );
    if( pdf->fp == NULL )
    {
       pdfm_error_code = PDFM_NOFILE;
       return NULL;
    }
    pdf->size = pdfm_getsize( pdf );
    pdf->raw_data = calloc( 1, sizeof(char) * pdf->size );
    uint64_t size = 0;
    if( pdf->size != ( size = fread( pdf->raw_data, sizeof(char), pdf->size, pdf->fp ) ) )
    {
       fprintf( stderr, "Error reading file %s. Expected %zu bytes. Read %zu bytes. Aborting.\n", pdf->filename, pdf->size, size );
       return NULL;
    }

    pdfm_loadversion( pdf );
    if( ( pdfm_error_code = process_raw_pdf_data( pdf ) ) != 0 )
    {
       free_pdf( pdf );
       return NULL;
    }

    return pdf;

}
