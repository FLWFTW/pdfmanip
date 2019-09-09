#include "pdfmanip.h"
#include "pdfmanip_dictionary.h"

static pdfm_otype identify_object_types( pdfm_o *obj );

const char *object_type_names[] = {
                                        [PDFM_UNKNOWN]     = "Unknown",
                                        [PDFM_INTEGER]     = "Integer",
                                        [PDFM_REAL]        = "Real",
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
    size_t len = strcspn( pch+1, "\n\r\\ \t/[<(" ); /*pch+1 because otherwise it will return 0 returning the / at the beginning of the name*/
    char *name = calloc( len+2, sizeof( char ) );/*len+1 to account for the / we skipped above*/
    memcpy( name, pch, len+1 );
    return name;
}

static LIST *read_array( char *pch )
{
    LIST *array = AllocList();
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

    for( ; len > 0; len-- )
    {

       if( pch[len] == ' ' )
          continue;
       switch( /*( identify_object_types(  ) */ pch[len] )
       {
          case PDFM_MAGICNUMBER:
             {
                break;
             }
          case PDFM_ARRAY:
             {
                break;
             }
          case PDFM_DICTIONARY:
             {
                break;
             }
          case PDFM_STRING:
             {
                break;
             }
          case PDFM_HEXSTRING:
             {
                break;
             }
          case PDFM_BOOLEAN:
             {
                break;
             }
          case PDFM_NULL:
             {
                break;
             }
          case PDFM_NAME:
             {
                break;
             }
          case PDFM_REAL:
             {
                break;
             }
          case PDFM_INTEGER:
             {
                break;
             }
          case PDFM_IR:
             {
                break;
             }
          case PDFM_STREAM:
             {
                break;
             }
          case PDFM_UNKNOWN:
             {
                break;
             }
       }
    }

    return array;
}



static pdfm_d *read_dictionary( char *pch )
{
   pdfm_d *dic = new_dictionary();
    while( *pch == '<' ) /*should be 2-- << [data] >>*/
        pch++;

    pch = skip_whitespace( pch );

    pdfm_dentry *entry;
    while( *pch != '>' && pch[1] != '>' )
    {
        entry = calloc( 1, sizeof( pdfm_dentry ) );
        entry->label = read_name( pch );
        pch = skip_whitespace( pch );
        pch += strlen( entry->label );
        pch = skip_whitespace( pch ); /*we should now be pointing at whatever's the subject of the name...*/

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
        else if( *pch == '<' && pch[1] == '<' )/*dictionary*/
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
            bool real = false;
            while( pch[len] != '/' && pch[len] != 'R' && pch[len] != '>' )
            {
               if( pch[len] == '.' )
                  real = true;
                len++;
            }
            if( pch[len] == '/' )/*this is a number;*/
            {
                uint64_t *val = calloc( 1, sizeof( uint64_t ) );
                *val = strtouint64_t( pch );
                entry->content = val;
                if( real == false )
                   entry->type = PDFM_INTEGER;
                else
                   entry->type = PDFM_REAL;
                pch += len;
            }
            else /*this is an indirect reference*/
            {
                len++;
                char *ir = calloc( len, sizeof( char ) );
                memcpy( ir, pch, len );
                entry->content = ir;
                entry->type = PDFM_IR;
                pch += strlen( ir );
            }
        }
        else if( !strincmp( pch, "true", 4 ) )
        {
           /*entry->content = true;*/
           entry->type = PDFM_BOOLEAN;
        }
        else if( !strincmp( pch, "false", 5 ) )
        {
           /*entry->content = false;*/
           entry->type = PDFM_BOOLEAN;
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

static pdfm_otype identify_object_types( pdfm_o *obj )
{
   if( obj->type != PDFM_UNKNOWN ) /* It's already been identified at some point... */
      return obj->type;
   /* Everything in this function needs to be modified... */
#ifdef REMOVEME
   size_t i = 0;
   switch( *pos )
   {
      case '>':
          {
              if( *(pos-1) == '>' )
                  return PDFM_DICTIONARY;
              else
                  return PDFM_HEXSTRING;
              break;
          }
      case ']':
          {
              return PDFM_ARRAY;
              break;
          }
      case 'R': /* Indirect reference format: <object number> <generation number> R */
          {
             if( *(pos-1) == ' ' )
                return PDFM_IR;
             break;
          }
      case ')':
          {
              return PDFM_STRING;
              break;
          }
      case '.': /* Yup, a number can technically be 1. not 1.0 */
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
          {
             if( *pos == '.' && *(pos - 1) < '0' && *(pos - 1) > '9' ) /* Not a floating point number */
                break;
             while( !isdigit( *(pos-i) ) )
                i++;
             if( *(pos-i) == '.' && isdigit( *(pos -i -1) ) )
                return PDFM_REAL;
             if( isspace( *(pos - i) ) )
                return PDFM_INTEGER;
             break;
          }
      case 'm':
             if( !strincmp( pos-8, "endstream", 8 ) )
                return PDFM_STREAM;
             /* fallthru */
      default:
          {
             /* If someone has a name named '/...endstream' this will break */
             /* Check if it's a name */
             while( *(pos-i - 1) != ' ' )
                i++;
             if( *(pos-i) == '/' )
                return PDFM_NAME;
             if( !strincmp( pos-4, "true", 4 ) || !strincmp( pos-5, "false", 5 ) )
                return PDFM_BOOLEAN;

             break;
          }
   }

   fprintf( stderr, "Unknown object type %c (%s)", *pos, pos-10 ); 
   abort( );
#endif
   return PDFM_SUCCESS;
}

static void load_xref_table( pdfm *pdf )
{
   unsigned long i;
   i = strtoul( pdf->pos, &pdf->pos, 0 ); 
   pdf->xref_count = strtoul( pdf->pos, &pdf->pos, 0 );

   pdfm_xref *xref;
   for( ; i < pdf->xref_count; i++ ) /*Each xref line is guaranteed by standard to be 20 bytes long*/
   {
       xref = calloc( 1, sizeof( pdfm_xref ) );
       xref->offset = strtouint64_t( pdf->pos );
       pdf->pos += 11;
       xref->generation = strtouint64_t( pdf->pos );
       pdf->pos += 7;
       xref->status = *pdf->pos;
       pdf->pos += 2;
       AttachSorted( xref, pdf->xref_table, xref_cmp );
   } /*11+7+2 = 20 bytes total*/
   pdf->pos++;
}

void nuke_comments( char *data, size_t len )
{
   size_t pos = 0;

   while( pos < len )
   {
      switch( data[pos] )
      {
         case '%':
            {
               data[pos] = ' ';
               while( data[pos] != '\n' && data[pos] != '\r' )
                  data[pos++] = ' ';
               break;
            }
         case '(': /*Start of string*/
         case '<':
            {
               if( data[pos] == '<' && data[pos+1] == '<' ) /* This is a dictionary, not a hex string */
               {
                  pos += 2; /* Skip past the << and continue searching for comments */
                  continue;
               }
               else
               {
                  char delim = data[pos];
                  int level = 0;
                  do
                  {
                     if( data[len] == delim )
                        level++;
                     else if( data[len] == delim )
                        level--;
                     pos++;
                  }while( level != 0 );
               }
               break;
            }
         case 's':
         case 'S': /* start of a stream? */
            {
               if( !strincmp( &data[pos], "stream", 6 ) )
               { /*skip until we get to 'endstream'*/
                  while( strincmp( &data[pos], "endstream", 9 ) )
                     pos++;
                  pos += 9;
               }
               break;
            }
         default:
            {
               break;
            }
      }
      pos++;
   }
}

static int read_base_objects( pdfm *pdf )
{

   ITERATOR Iter;
   pdfm_xref *next;
   pdfm_o *obj;
   pdfm_xref *xref;
   char *pos;
   AttachIterator( &Iter, pdf->xref_table );
   while( ( xref = NextInList( &Iter ) ) != NULL )
   {
      obj = calloc( 1, sizeof( pdfm_o ) );
      next = peekNextInList( &Iter );
      if( next == NULL )
      {
         size_t p = pdf->size;/*Point to the last byte in the file*/
         while( strncmp( &pdf->raw_data[p], "endobj", 6 ) ) /*Find the end of the last object*/
            p--;
         obj->size = p - xref->offset + 6;/*Add the 'endobj' marker back on*/
      }
      else
         obj->size = next->offset - xref->offset;
      obj->raw_data = calloc( obj->size, sizeof( char ) );
      obj->offset = xref->offset;
      memcpy( obj->raw_data, pdf->raw_data+xref->offset, obj->size );
      /* Read in the object and generation number */
      pos = obj->raw_data;
      obj->object_number = strtoul( pos, &pos, 0 );
      obj->generation_number = strtoul( pos, &pos, 0 );

      /*Make sure this isn't the magic number object*/
      if( !strncmp( obj->raw_data, pdf->raw_data, 6 ) )
      {
         obj->type = PDFM_MAGICNUMBER;
         continue;
      }

      nuke_comments( obj->raw_data, obj->size );

      /* Trim the raw data down */
      while( strncmp( pos, "obj", 3 ) )
         pos++;
      pos += 3; /* Skip past 'obj' */
      pos = skip_whitespace( pos );
      obj->size = obj->size - ( pos - &(*obj->raw_data) ); /*Trim the size down*/
      memmove( obj->raw_data, pos, obj->size ); /*Shift the data we want to keep to the beginning of the raw data buffer*/
      obj->raw_data[obj->size] = '\0';
      pos = &obj->raw_data[obj->size];
      while( strncmp( pos, "endobj", 6 ) ) /*Trim off 'endobj'*/
         pos--;
      while( *pos == '\0' || *pos == '\n' || *pos == '\r' || *pos == ' ' || *pos == '\t' ) /*Any blanks*/
         pos--;
      obj->size = pos - &(*obj->raw_data); /*shrink the size down to cut off the fluff*/
      obj->raw_data = realloc( obj->raw_data, obj->size ); /*Reduce the amount of memory we have allocated*/
      obj->raw_data[obj->size] = '\0'; /*And make sure it's NULL terminated*/

      /* Identify the object type */
      obj->type = identify_object_types( obj );
      AppendToList( obj, pdf->objects );
  }
  DetachIterator( &Iter );
  return PDFM_SUCCESS;
}

static int process_raw_pdf_data( pdfm *pdf )
{
   char *pch = pdf->raw_data;
   pdf->pos = pch + pdf->size;

   if( pch[0] != '%' || pch[1] != 'P' || pch[2] != 'D' || pch[3] != 'F' ) /*No pdf version number*/
      return PDFM_NOVERSION;
   size_t i = 0;
   for( i = pdf->size; i > 0; i-- )
   {
       if( !strncmp( pch + i, "%%EOF", 5 ) )
           break;
   }
   if( i == 0 ) /*No EOF marker*/
      return PDFM_NOEOF;

   while( strncmp( pdf->pos, "startxref", 9 ) ) /*find the reference location of the xref table*/
      pdf->pos--;

   pdf->pos += 9;/*point to after 'startxref' in the file.*/
   pdf->xref_location = strtouint64_t( pdf->pos );
   pdf->pos = pch + pdf->xref_location;

   if( strncmp( pdf->pos, "xref", 4 ) ) /*xref table not found*/
      return PDFM_NOXREF;

   pdf->pos += 5;
   while( strncmp( pdf->pos, "trailer", 7 ) )
   {
      load_xref_table( pdf );
      pdf->pos = skip_spaces( pdf->pos );
      /* trailer is formatted like this:
       * n N
       * 00000000001 00000 f
       * 00000000020 00000 f
       * ...
       * where n is the 'counting from' number and N is the number of rows to count. Each row is 20 bytes long. But there can be multiple segments of n N/20 byte row segments, IE:
       * 0 3
       * 0000000000 00000 f
       * 0000000010 00001 f
       * 0000000087 00001 f
       * 0 2
       * 0000000103 00001 f
       * 0000003020 00001 f
       * trailer
       * */
      if( *pdf->pos < 0x30 || *pdf->pos > 0x39 ) /*Fail safe so we don't get stuck in an endless loop if there is no trailer right after the xref table (There should be*/
         break;
   }
   if( strncmp( pdf->pos, "trailer", 7 ) )
   {
      fprintf( stderr, "%s", pdf->pos );
      return PDFM_NOTRAILER;
   }
   pdf->pos += 7; /*Skip 'trailer'*/
   pdf->pos = skip_whitespace( pdf->pos );/*in case there are any extra spaces*/
    pdf->trailer = read_dictionary( pdf->pos );
   if( pdf->trailer == NULL )
       return PDFM_UNSPECIFIED;

   /*Process all the objects...*/
   int ret;
   ret = read_base_objects( pdf );
   if( ret != PDFM_SUCCESS )
       return ret;
   /*ret = interpret_base_objects( pdf );*/
   if( ret != PDFM_SUCCESS )
      return ret;

   return ret;
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
