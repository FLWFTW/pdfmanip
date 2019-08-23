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

