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
    ret[0] = orig[0];/*probably a more eloquent way of copying that first byte, but this works.*/

    return ret;
}

int strincmp( char *s1, char *s2, size_t n )
{
    while ( n && *s1 && ( toupper(*s1) == toupper(*s2) ) )
    {
        ++s1;
        ++s2;
        --n;
    }
    if ( n == 0 )
    {
        return 0;
    }
    else
    {
        return ( *(unsigned char *)(s1) - *(unsigned char *)(s2) );
    }
}
