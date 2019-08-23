#include <ctype.h>
#include <stdint.h>

char *token( char *pch, char *tok )
{
  /* skip leading spaces */
  while (isspace(*pch) || iscntrl(*pch))
    pch++; 

  /* copy the beginning of the string */
  while (*pch != '\0')
  {
    /* have we reached the end of the first word ? */
    if( isspace( *pch ) || iscntrl( *pch ) )
    {
      pch++;
      break;
    }

    /* copy one char */
    *tok++ = *pch++;
  }

  /* terminate string */
  *tok = '\0';

  /* skip past any leftover spaces */
  while( isspace( *pch ) || iscntrl( *pch ) )
    pch++;

  /* return the leftovers */
  return pch;
}

uint64_t strtouint64_t( char *str )
{
   uint64_t ret = 0;
   while( !isdigit( *str ) )
      str++;
   ret = *str - '0';
   while( isdigit( *str ) )
   {
      ret *= 10;
      ret += *str - '0';
      str++;
   }

   return ret;
}

