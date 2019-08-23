#ifndef __PDFM_ERROR_H__
#define __PDFM_ERROR_H__


extern const char *pdfm_error_message[];
enum e_pdfm_error_code { PDFM_SUCCESS, PDFM_UNSPECIFIED, PDFM_NOVERSION, PDFM_NOEOF, PDFM_NOXREF, PDFM_NOFILE, PDFM_NOTRAILER };

typedef enum e_pdfm_error_code pdfm_error;

#endif
