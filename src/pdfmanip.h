#ifndef __PDFFT_H__
#define __PDFFT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "list.h"
#include "utils.h"

typedef struct s_pdfft pdfft;
typedef struct s_pdfft_indirect_object pdfft_io;
typedef struct e_pdf_otype pdfft_otype;

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

enum e_pdfft_otype { pdf_boolean = 0, pdf_integer, pdf_real, pdf_string, pdf_name, pdf_array, pdf_dictionary, pdf_stream, pdf_null };

struct s_pdfft
{
    int          version_major;
    int          version_minor;
    char       * version_string;

    size_t       size;
    FILE       * fp;
    
    char       * filename;
    char       * raw_data;
    LIST       * objects;
};

struct s_pdfft_object
{
   unsigned int object_number;
   unsigned int generation_number;

   LIST         * contents;
};

pdfft *load_pdf( char *filename );
void   free_pdf( pdfft *pdf );
#endif
