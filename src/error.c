#include "error.h"

const char  *pdfm_error_message[] = { [PDFM_SUCCESS]     = "Success!",
                              /*1*/   [PDFM_UNSPECIFIED] = "Unspecified error.",
                              /*2*/   [PDFM_NOVERSION]   = "PDF Version (%pdf-n.n) not found at beginning of file.",
                              /*3*/   [PDFM_NOEOF]       = "PDF End of File marker (%%EOF) not found at end of file.",
                              /*4*/   [PDFM_NOXREF]      = "PDF xref table not found.",
                              /*5*/   [PDFM_NOFILE]      = "File not found.",
                              /*6*/   [PDFM_NOTRAILER]  = "Trailer section not found.",
                                      0};

