#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdint.h>

uint64_t strtouint64_t( char *str )
{
   uint64_t ret = 0;
   while( !isdigit( *str ) )
      str++;
   ret = *str - '0';
   str++;
   while( isdigit( *str ) )
   {
      ret *= 10;
      ret += *str - '0';
      str++;
   }

   return ret;
}

char *strdup( char *orig )
{
    uint64_t len = 0;
    while( orig[len] != '\0' )
        len++;

    char *ret = malloc( sizeof( char ) * len );
    do{
        ret[len] = orig[len];
        len--;
    }while( len > 0 );
    ret[0] = orig[0];//probably a more eloquent way of copying that first byte, but this works.

    return ret;
}

/*copied from http://www.cse.yorku.ca/~oz/hash.html*/
uint64_t sdbm( char *str )
{
    uint64_t hash = 0;
    int c;

    while ( ( c = *str++ ) != 0x00 )
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

