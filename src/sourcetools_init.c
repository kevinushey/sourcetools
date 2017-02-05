// This file was automatically generated.

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

SEXP sourcetools_code_complete_at(SEXP, SEXP);
SEXP sourcetools_performs_nse(SEXP);
SEXP sourcetools_parse_string(SEXP);
SEXP sourcetools_diagnose_string(SEXP);
SEXP sourcetools_read(SEXP);
SEXP sourcetools_read_lines(SEXP);
SEXP sourcetools_read_bytes(SEXP);
SEXP sourcetools_read_lines_bytes(SEXP);
SEXP sourcetools_print_multibyte(SEXP);
SEXP sourcetools_print_utf8(SEXP);
SEXP sourcetools_tokenize_file(SEXP);
SEXP sourcetools_tokenize_string(SEXP);
SEXP sourcetools_validate_syntax(SEXP);

static R_CallMethodDef callMethods[] = {
	{"C_sourcetools_code_complete_at", (DL_FUNC) &sourcetools_code_complete_at, 2},
	{"C_sourcetools_performs_nse", (DL_FUNC) &sourcetools_performs_nse, 1},
	{"C_sourcetools_parse_string", (DL_FUNC) &sourcetools_parse_string, 1},
	{"C_sourcetools_diagnose_string", (DL_FUNC) &sourcetools_diagnose_string, 1},
	{"C_sourcetools_read", (DL_FUNC) &sourcetools_read, 1},
	{"C_sourcetools_read_lines", (DL_FUNC) &sourcetools_read_lines, 1},
	{"C_sourcetools_read_bytes", (DL_FUNC) &sourcetools_read_bytes, 1},
	{"C_sourcetools_read_lines_bytes", (DL_FUNC) &sourcetools_read_lines_bytes, 1},
	{"C_sourcetools_print_multibyte", (DL_FUNC) &sourcetools_print_multibyte, 1},
	{"C_sourcetools_print_utf8", (DL_FUNC) &sourcetools_print_utf8, 1},
	{"C_sourcetools_tokenize_file", (DL_FUNC) &sourcetools_tokenize_file, 1},
	{"C_sourcetools_tokenize_string", (DL_FUNC) &sourcetools_tokenize_string, 1},
	{"C_sourcetools_validate_syntax", (DL_FUNC) &sourcetools_validate_syntax, 1},
	{NULL, NULL, 0}
};


void R_init_sourcetools(DllInfo* info) {
	R_registerRoutines(info, NULL, callMethods, NULL, NULL);
	R_useDynamicSymbols(info, FALSE);
}
