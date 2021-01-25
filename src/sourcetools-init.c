#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* FIXME: 
   Check these declarations against the C/Fortran source code.
*/

/* .Call calls */
extern SEXP run_testthat_tests();
extern SEXP sourcetools_diagnose_string(SEXP);
extern SEXP sourcetools_parse_string(SEXP);
extern SEXP sourcetools_performs_nse(SEXP);
extern SEXP sourcetools_read(SEXP);
extern SEXP sourcetools_read_bytes(SEXP);
extern SEXP sourcetools_read_lines(SEXP);
extern SEXP sourcetools_read_lines_bytes(SEXP);
extern SEXP sourcetools_tokenize_file(SEXP);
extern SEXP sourcetools_tokenize_string(SEXP);
extern SEXP sourcetools_validate_syntax(SEXP);

static const R_CallMethodDef CallEntries[] = {
    {"run_testthat_tests",           (DL_FUNC) &run_testthat_tests,           0},
    {"sourcetools_diagnose_string",  (DL_FUNC) &sourcetools_diagnose_string,  1},
    {"sourcetools_parse_string",     (DL_FUNC) &sourcetools_parse_string,     1},
    {"sourcetools_performs_nse",     (DL_FUNC) &sourcetools_performs_nse,     1},
    {"sourcetools_read",             (DL_FUNC) &sourcetools_read,             1},
    {"sourcetools_read_bytes",       (DL_FUNC) &sourcetools_read_bytes,       1},
    {"sourcetools_read_lines",       (DL_FUNC) &sourcetools_read_lines,       1},
    {"sourcetools_read_lines_bytes", (DL_FUNC) &sourcetools_read_lines_bytes, 1},
    {"sourcetools_tokenize_file",    (DL_FUNC) &sourcetools_tokenize_file,    1},
    {"sourcetools_tokenize_string",  (DL_FUNC) &sourcetools_tokenize_string,  1},
    {"sourcetools_validate_syntax",  (DL_FUNC) &sourcetools_validate_syntax,  1},
    {NULL, NULL, 0}
};

void R_init_sourcetools(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
