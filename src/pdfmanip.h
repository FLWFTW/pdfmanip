#ifndef __PDFMANIP_H__
#define __PDFMANIP_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "list.h"
#include "utils.h"
#include "error.h"

typedef struct s_pdfmanip pdfm;
typedef struct s_pdfmanip_object pdfm_o;

pdfm_error pdfm_error_code;

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

struct s_pdfmanip
{
    int          version_major;
    int          version_minor;
    char       * version_string;

    uint64_t     size;
    FILE       * fp;
    
    char       * filename;
    char       * raw_data;
    LIST       * objects;
    uint64_t     xref_location;
    uint64_t     xref_count;
};

struct s_pdfmanip_object
{
   uint64_t     object_number;
   uint64_t     generation_number;

   uint64_t     offset;
   char         status;

   LIST         * contents;
};

pdfm *load_pdf( char *filename );
void   free_pdf( pdfm *pdf );
#endif
