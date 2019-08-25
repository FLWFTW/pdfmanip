#include <stdio.h>
#include "list.h"

struct test
{
    int num;
    char name[256];
};

int comp( const void *left, const void *right )
{
    struct test *l = (struct test*)left;
    struct test *r = (struct test*)right;

    return ( l->num - r->num );
}

int main( int numArgs, char **argList )
{
    LIST *list = AllocList();
    struct test array[] = {
        { 5, "Five\0" },
        { 4, "Four\0" },
        { 3, "Three\0" },
        { 2, "Two\0" },
        { 1, "One\0" },
        0,
    };

    for( int i = 0; i < 5; i++ )
    {
        AttachSorted( &array[i], list, comp );
    }

    ITERATOR Iter;
    AttachIterator( &Iter, list );
    struct test *t;
    while( ( t = NextInList( &Iter ) ) != NULL )
    {
        fprintf( stdout, "%i. %s.\n", t->num, t->name );
    }
    DetachIterator( &Iter );

    return 0;
}
