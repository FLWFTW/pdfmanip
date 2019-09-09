/* file: list.c
 *
 * The implementation of a basic double-linked list
 */

#include <stdlib.h>

#include "list.h"

/* local procedures */
void  FreeCell        ( CELL *pCell, LIST *pList );
CELL *AllocCell       ( void );
void  InvalidateCell  ( CELL *pCell );

LIST *AllocList()
{
  LIST *pList;

  pList = malloc(sizeof(*pList));
  pList->_pFirstCell = NULL;
  pList->_pLastCell = NULL;
  pList->_iterators = 0;
  pList->_valid = 1;
  pList->_size = 0;

  return pList;
}

void AttachIterator(ITERATOR *pIter, LIST *pList)
{
  pIter->_pList = pList;

  if (pList != NULL)
  {
    pList->_iterators++;
    pIter->_pCell = pList->_pFirstCell;
  }
  else
    pIter->_pCell = NULL;
}

CELL *AllocCell()
{
  CELL *pCell;

  pCell = calloc( 1, sizeof(*pCell) );
  pCell->_valid = 1;

  return pCell;
}

void AttachToList(void *pContent, LIST *pList)
{
  CELL *pCell = NULL;

  for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pCell->_pNextCell)
  {
    if (pCell && !pCell->_valid)
      continue;

    if (pCell->_pContent == pContent)
    {
        return;
    }
  }

  pCell = AllocCell();
  pCell->_pContent = pContent;
  pCell->_pNextCell = pList->_pFirstCell;

  if (pList->_pFirstCell != NULL)
    pList->_pFirstCell->_pPrevCell = pCell;
  if (pList->_pLastCell == NULL)
    pList->_pLastCell = pCell;
  pList->_pLastCell->_pNextCell = NULL;

  pList->_pFirstCell = pCell;

  pList->_size++;
}

/**
 * Merges listA into listB, empties listA 
 */
void MergeList( LIST *listA, LIST *listB )
{
   ITERATOR Iter;
   void *item;

   AttachIterator( &Iter, listA );
   while( ( item = NextInList( &Iter ) ) != NULL )
   {
      AttachToList( item, listB );
      DetachFromList( item, listA );
   }
   DetachIterator( &Iter );

   return;
}

void DetachFromList(void *pContent, LIST *pList)
{
  CELL *pCell;

  for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pCell->_pNextCell)
  {
    if (pCell->_pContent == pContent)
    {
      if (pList->_iterators > 0)
        InvalidateCell(pCell);
      else
        FreeCell(pCell, pList);
      pList->_size--;
      return;
    }
  }
}

void DetachIterator(ITERATOR *pIter)
{
  LIST *pList = pIter->_pList;

  if (pList != NULL)
  {
    pList->_iterators--;

    /* if this is the only iterator, free all invalid cells */
    if (pList->_iterators <= 0)
    {
      CELL *pCell, *pNextCell;

      for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pNextCell)
      {
        pNextCell = pCell->_pNextCell;

        if (!pCell->_valid)
          FreeCell(pCell, pList);
      }

      if (!pList->_valid)
        FreeList(pList);
    }
  }
}

void FreeList(LIST *pList)
{
  CELL *pCell, *pNextCell;

  /* if we have any unfinished iterators, wait for later */
  if (pList->_iterators > 0)
  {
    pList->_valid = 0;
    return;
  }

  for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pNextCell)
  {
    pNextCell = pCell->_pNextCell;

    FreeCell(pCell, pList);
  }

  free(pList);
}

void FreeCell(CELL *pCell, LIST *pList)
{
  if (pList->_pFirstCell == pCell)
    pList->_pFirstCell = pCell->_pNextCell;

  if (pList->_pLastCell == pCell)
    pList->_pLastCell = pCell->_pPrevCell;

  if (pCell->_pPrevCell != NULL)
    pCell->_pPrevCell->_pNextCell = pCell->_pNextCell;

  if (pCell->_pNextCell != NULL) 
    pCell->_pNextCell->_pPrevCell = pCell->_pPrevCell;

  free(pCell);
}

void InvalidateCell(CELL *pCell)
{
  pCell->_valid = 0;
}

void *NextInList(ITERATOR *pIter)
{
  void *pContent = NULL;

  /* skip invalid cells */
  while (pIter->_pCell != NULL && !pIter->_pCell->_valid)
  {
    pIter->_pCell = pIter->_pCell->_pNextCell;
  }

  if (pIter->_pCell != NULL)
  {
    pContent = pIter->_pCell->_pContent;
    pIter->_pCell = pIter->_pCell->_pNextCell;
  }

  return pContent;
}

void *peekNextInList( ITERATOR *iter )
{
    void *pContent = NULL;
    ITERATOR *pIter = iter;
    while( pIter->_pCell != NULL && !pIter->_pCell->_valid )
        pIter->_pCell = pIter->_pCell->_pNextCell;

    if( pIter->_pCell != NULL )
    {
        pContent = pIter->_pCell->_pContent;
    }
        return pContent;
}

int SizeOfList(LIST *pList)
{
  return pList->_size;
}

void *NthFromList( LIST *pList, size_t n )
{
   void *ptr;
   size_t i;
   if( !pList )
      return NULL;
   if( n > SizeOfList( pList ) )
      return NULL;

   ITERATOR iter;
   AttachIterator( &iter, pList );
   for( i = 0; i <= n; i++ )
   {
      ptr = NextInList( &iter );
   }
   DetachIterator( &iter );
   
   return ptr;
}

void AttachSorted( void *pContent, LIST *pList, int (*cmp)(const void *, const void *) )
{
    CELL *pCell = NULL, *nCell = NULL;

    for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pCell->_pNextCell)
    {
        if (pCell && !pCell->_valid)
            continue;

        if (pCell->_pContent == pContent)
        {
            /*Do not attach if its already in the list*/
            return;
        }

        if( (*cmp)(pCell->_pContent, pContent) > 0 )
            break;
    }

    nCell = AllocCell();
    nCell->_pContent = pContent;
    if( pCell )/*Not inserting at the very end of the list*/
    {
        if( pList->_pFirstCell == pCell ) /*We're inserting at the very beginning of the list*/
        {
            nCell->_pNextCell = pList->_pFirstCell;

            if (pList->_pFirstCell != NULL)
                pList->_pFirstCell->_pPrevCell = nCell;
            if (pList->_pLastCell == NULL)
                pList->_pLastCell = nCell;
            pList->_pLastCell->_pNextCell = NULL;

            pList->_pFirstCell = nCell;
        }
        else/*Inserting somewhere in the middle of the list*/
        {
            nCell->_pPrevCell = pCell->_pPrevCell;
            pCell->_pPrevCell = nCell;
            nCell->_pNextCell = pCell;
            nCell->_pPrevCell->_pNextCell = nCell;
        }
    }
    else /*Appending to the end of the list.*/
    {
       nCell->_pPrevCell = pList->_pLastCell;
       if( pList->_pLastCell )
       {
          pList->_pLastCell->_pNextCell = nCell;
       }
       pList->_pLastCell = nCell;
       if( pList->_pFirstCell == NULL )
       {
          pList->_pFirstCell = nCell;
       }
    }

    pList->_size++;
}

void AppendToList( void *pContent, LIST *pList )
{
   CELL *pCell = NULL;

   for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pCell->_pNextCell)
   {
      if (pCell && !pCell->_valid)
      continue;

      if (pCell->_pContent == pContent)
      {
         /*Do not attach if its already in the list*/
         return;
      }
   }

   pCell = AllocCell();
   pCell->_pContent = pContent;
   pCell->_pPrevCell = pList->_pLastCell;
   if( pList->_pLastCell )
   {
      pList->_pLastCell->_pNextCell = pCell;
   }
   pList->_pLastCell = pCell;
   if( pList->_pFirstCell == NULL )
      pList->_pFirstCell = pCell;

   pList->_size++;
}

