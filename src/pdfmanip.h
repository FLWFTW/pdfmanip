#ifndef __PDFFT_H__
#define __PDFFT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "list.h"
#include "utils.h"

extern char *pdfm_error_message[];
extern int   pdfm_error_code;

typedef struct s_pdfmanip pdfm;
typedef struct s_pdfmanip_object pdfm_o;
typedef enum   e_pdfmanip_otype pdfm_otype;

#define WSP_NL    0x00
#define WSP_HT    0x09
#define WSP_LF    0x0A
#define WSP_FF    0X0C
#define WSP_CR    0x0D
#define WSP_SP    0x20

#define DLM_LP    0x28
#define DLM_RP    0x29
#define DLM_LT    0x3C
#define DLM_GT    0x3E
#define DLM_LB    0x5B
#define DLM_RB    0x5D
#define DLM_LC    0x7B
#define DLM_RC    0x7D
#define DLM_SD    0x2F
#define DLM_PC    0x25

enum e_pdfmanip_otype { pdf_boolean = 0, pdf_integer, pdf_real, pdf_string, pdf_name, pdf_array, pdf_dictionary, pdf_stream, pdf_null };

struct s_pdfmanip
{
    int          version_major;
    int          version_minor;
    char       * version_string;

    size_t       size;
    FILE       * fp;
    
    char       * filename;
    char       * raw_data;
    LIST       * objects;
    size_t       xref_location;
    size_t       xref_count;
};

struct s_pdfmanip_object
{
   unsigned int object_number;
   unsigned int generation_number;

   size_t       offset;
   char         status;

   LIST         * contents;
};

pdfm *load_pdf( char *filename );
void   free_pdf( pdfm *pdf );
#endif
