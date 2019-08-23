unsigned int pdfm_error_code;
const char  *pdfm_error_message[] = { "Success!",
                              /*1*/   "Unspecified error.",
                              /*2*/   "PDF Version (%pdf-n.n) not found at beginning of file.",
                              /*3*/   "PDF End of File marker (%%EOF) not found at end of file.",
                              /*4*/   "PDF xref table not found.",
                              /*5*/   "File not found.",
                                      0};

