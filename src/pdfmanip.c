#include "pdfmanip.h"
#include "pdfmanip_dictionary.h"

//static pdfm_o *new_pdfmo();

const char *object_type_names[] = {
                                        [PDFM_NUMBER]      = "Number",
                                        [PDFM_IR]          = "Indirect Reference",
                                        [PDFM_NAME]        = "Name",
                                        [PDFM_DICTIONARY]  = "Dictionary",
                                        [PDFM_ARRAY]       = "Array",
                                        [PDFM_STRING]      = "String",
                                        [PDFM_HEXSTRING]   = "Hex String",
                                        [PDFM_STREAM]      = "Stream",
                                        [PDFM_BOOLEAN]     = "Boolean",
                                        [PDFM_NULL]        = "Null",
                                        [PDFM_MAGICNUMBER] = "Magic Number",
                                        0};

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

static char *skip_spaces( char *str )
{
   while( isspace( *str ) )
      str++;
   return str;
}

static char *skip_whitespace( char *str )
{
   while( *str == 0x00 || isspace( *str ) || *str == '\n' || *str == '\r' )
      str++;
   return str;
}

static char *read_name( char *pch )
{
    size_t len = strcspn( pch+1, "\n\r\\ \t/[<(" );//pch+1 because otherwise it will return 0 returning the / at the beginning of the name
    char *name = calloc( len+2, sizeof( char ) );//len+1 to account for the / we skipped above
    memcpy( name, pch, len+1 );
    return name;
}

static char *read_array( char *pch )
{
    size_t len = 0;
    int level = 0;
    do
    {
        if( pch[len] == '[' )
            level++;
        else if( pch[len] == ']' )
            level--;
        len++;
    }while( level != 0 );
    char *array = calloc( len, sizeof( char ) );
    memcpy( array, pch, len );

    return array;
}

static pdfm_d *read_dictionary( char *pch )
{
   pdfm_d *dic = new_dictionary();
    while( *pch == '<' ) //should be 2-- << [data] >>
        pch++;

    pch = skip_whitespace( pch );

    pdfm_dentry *entry;
    while( *pch != '>' && pch[1] != '>' )
    {
        entry = calloc( 1, sizeof( pdfm_dentry ) );
        entry->label = read_name( pch );
        pch = skip_whitespace( pch );
        pch += strlen( entry->label );
        pch = skip_whitespace( pch ); //we should now be pointing at whatever's the subject of the name...

        if( *pch == '/' )
        {
            entry->content = read_name( pch );
            pch += strlen( entry->content );
            pch = skip_whitespace( pch );
            entry->type = PDFM_NAME;
        }
        else if( *pch == '[' )
        {
            entry->content = read_array( pch );
            pch += strlen( entry->content );
            pch = skip_whitespace( pch );
            entry->type = PDFM_ARRAY;
        }
        else if( *pch == '<' && pch[1] == '<' )//dictionary
        {
            entry->content = read_dictionary( pch );
            entry->type = PDFM_DICTIONARY;
        }
        else if( *pch == '<' )
        {
            entry->type = PDFM_HEXSTRING;
        }
        else if( *pch == '(' )
        {
            entry->type = PDFM_STRING;
        }
        else if( isdigit( *pch ) )
        {
            size_t len = 0;
            while( pch[len] != '/' && pch[len] != 'R' && pch[len] != '>' )
                len++;
            if( pch[len] == '/' )//this is a number;
            {
                uint64_t *val = calloc( 1, sizeof( uint64_t ) );
                *val = strtouint64_t( pch );
                entry->content = val;
                entry->type = PDFM_NUMBER;
                pch += len;
            }
            else //this is an indirect reference
            {
                len++;
                char *ir = calloc( len, sizeof( char ) );
                memcpy( ir, pch, len );
                entry->content = ir;
                entry->type = PDFM_IR;
                pch += strlen( ir );
            }
        }
        else
        {
            fprintf( stderr, "Malformed pdf." );
            abort();
        }
        dictionary_add( entry, dic );
        pch = skip_whitespace( pch );
    }

    return dic;
}

static int xref_cmp( const void *left, const void *right )
{
    pdfm_xref *l = (pdfm_xref *)left;
    pdfm_xref *r = (pdfm_xref *)right;

    if( l->offset == r->offset )
        return 0;
    else if( l->offset < r->offset )
        return -1;
    else
        return 1;
}


static int identify_object_types( pdfm *pdf )
{
    ITERATOR Iter;
    pdfm_o *obj;
    char *pos;
    AttachIterator( &Iter, pdf->objects );
    while( ( obj = NextInList( &Iter ) ) != NULL )
    {
        if( !strncmp( obj->raw_data, pdf->raw_data, 6 ) )//The first object is the magic number, lets skip that.
        {
            obj->type = PDFM_MAGICNUMBER;
            continue;
        }
        pos = obj->raw_data+obj->size;
        while( strncmp( pos, "endobj", 6 ) )
            pos--;
        pos--;//were at 'e', now at whatever's before "endobj"
        while( pos != &obj->raw_data[0] && ( *pos == ' ' || *pos == '\t' || *pos == '\n' || *pos == '\r' ) )
            pos--;
        switch( *pos )
        {
            case '>':
                {
                    if( *(pos-1) == '>' )
                        obj->type = PDFM_DICTIONARY;
                    else
                        obj->type = PDFM_HEXSTRING;
                    break;
                }
            case ']':
                {
                    obj->type = PDFM_ARRAY;
                    break;
                }
            case 'm':
            case 'M':
                {
                    if( !strncmp( pos-8, "endstream", 8 ) )
                        obj->type = PDFM_STREAM;
                    else
                        return PDFM_UNSPECIFIED;
                    break;
                }
            case ')':
                {
                    obj->type = PDFM_STRING;
                    break;
                }
            default:
                {
                    return PDFM_UNSPECIFIED;
                    break;
                }
        }

    }
    DetachIterator( &Iter );


    return PDFM_SUCCESS;
}

static int process_raw_pdf_data( pdfm *pdf )
{
   char *pch = pdf->raw_data;
   pdf->pos = pch + pdf->size;

   if( pch[0] != '%' || pch[1] != 'P' || pch[2] != 'D' || pch[3] != 'F' ) //No pdf version number
      return PDFM_NOVERSION;
   size_t i = 0;
   for( i = pdf->size; i > 0; i-- )
   {
       if( !strncmp( pch + i, "%%EOF", 5 ) )
           break;
   }
   if( i == 0 ) //No EOF marker
      return PDFM_NOEOF;

   while( strncmp( pdf->pos, "startxref", 9 ) ) //find the reference location of the xref table
      pdf->pos--;

   pdf->pos += 9;//point to after 'startxref' in the file.
   pdf->xref_location = strtoull( pdf->pos, NULL, 0 );
   pdf->pos = pch + pdf->xref_location;

   if( strncmp( pdf->pos, "xref", 4 ) ) //xref table not found
      return PDFM_NOXREF;

   pdf->pos += 5;
   strtoul( pdf->pos, &pdf->pos, 0 );
   pdf->xref_count = strtoull( pdf->pos, &pdf->pos, 0 );

   pdfm_xref *xref;
   for( i = 0; i < pdf->xref_count; i++ )
   {
       xref = calloc( 1, sizeof( pdfm_xref ) );
       xref->offset = strtouint64_t( pdf->pos );
       pdf->pos += 11;
       xref->generation = strtouint64_t( pdf->pos );
       pdf->pos += 7;
       xref->status = *pdf->pos;
       pdf->pos += 2;
       AttachSorted( xref, pdf->xref_table, xref_cmp );
   }
   pdf->pos++;

   pdf->pos = skip_spaces( pdf->pos );
   if( strncmp( pdf->pos, "trailer", 7 ) )
   {
      fprintf( stderr, "%s", pdf->pos );
      return PDFM_NOTRAILER;
   }
   pdf->pos += 7; //Skip 'trailer'
   pdf->pos = skip_whitespace( pdf->pos );//in case there are any extra spaces
    pdf->trailer = read_dictionary( pdf->pos );
   if( pdf->trailer == NULL )
       return PDFM_UNSPECIFIED;

   ITERATOR Iter;
   pdfm_xref *next;
   pdfm_o *obj;
   AttachIterator( &Iter, pdf->xref_table );
   while( ( xref = NextInList( &Iter ) ) != NULL )
   {
       obj = calloc( 1, sizeof( pdfm_o ) );
       next = peekNextInList( &Iter );
       if( next == NULL )
       {
           size_t p = pdf->size;//Point to the last byte in the file
           while( strncmp( &pdf->raw_data[p], "endobj", 6 ) ) //Find the end of the last object
               p--;
           obj->size = p - xref->offset + 6;//Add the 'endobj' marker back on
       }
       else
           obj->size = next->offset - xref->offset;
       obj->raw_data = calloc( obj->size, sizeof( char ) );
       obj->offset = xref->offset;
       memcpy( obj->raw_data, pdf->raw_data+xref->offset, obj->size );

       AppendToList( obj, pdf->objects );

   }
   DetachIterator( &Iter );

   //Process all the objects...
   int ret = identify_object_types( pdf );
   if( ret != PDFM_SUCCESS )
       return ret;

   return 0;
}

pdfm *new_pdf()
{
   pdfm *pdf = calloc( 1, sizeof( pdfm ) );
   if( pdf == NULL )
      return NULL;
   pdf->objects = AllocList();
   pdf->xref_table = AllocList();

   return pdf;
}

/*
static pdfm_o *new_pdfmo()
{
   pdfm_o *o = calloc( 1, sizeof( pdfm_o ) );
   o->contents = AllocList();

   return o;
}
*/

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
