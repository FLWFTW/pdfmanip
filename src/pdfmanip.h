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
typedef struct s_pdfmanip_dictionary pdfm_d;
typedef struct s_pdfmanip_xref pdfm_xref;
typedef enum   e_pdfmanip_otype pdfm_otype;

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

enum e_pdfmanip_otype { PDFM_NUMBER, PDFM_IR, PDFM_NAME, PDFM_DICTIONARY, PDFM_ARRAY, PDFM_STRING, PDFM_HEXSTRING, PDFM_STREAM, PDFM_BOOLEAN, PDFM_NULL };

struct s_pdfmanip
{
    int          version_major;
    int          version_minor;
    char       * version_string;
    char       * filename;
    char       * raw_data;
    uint64_t     size;
    uint64_t     xref_location;
    uint64_t     xref_count;
    FILE       * fp;
    LIST       * objects;
    LIST       * xref_table;
    LIST       * trailer;
};

struct s_pdfmanip_xref
{
    uint64_t     offset;
    uint64_t     generation;
    char         status;
};

struct s_pdfmanip_content
{
    int          type;
    void       * content;
};

struct s_pdfmanip_dictionary
{
    pdfm_otype   type;
    uint64_t     hash;
    char       * label;
    void       * content;
};

struct s_pdfmanip_object
{
   uint64_t      object_number;
   uint64_t      generation_number;
   uint64_t      offset;
   char          status;
   pdfm_otype    type;
   LIST        * contents;
};

pdfm *load_pdf( char *filename );
void   free_pdf( pdfm *pdf );
#endif
