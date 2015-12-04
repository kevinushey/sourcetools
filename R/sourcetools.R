#' @useDynLib sourcetools
NULL

#' Read a file
#'
#' Read the contents of a file into a string.
#'
#' @param path A file path.
#'
#' @export
read <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call("sourcetools_read", path, PACKAGE = "sourcetools")
}

#' Tokenize an R File
#'
#' Read and tokenize an R file.
#'
#' @param path A file path.
#'
#' @export
tokenize_file <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call("sourcetools_tokenize_file", path, PACKAGE = "sourcetools")
}

#' Tokenize an R String
#'
#' Read and tokenize an R string.
#'
#' @param string A character vector (of length one).
#' @export
tokenize_string <- function(string) {
  .Call("sourcetools_tokenize_string", string, PACKAGE = "sourcetools")
}

#' Find Syntax Errors
#'
#' Find syntax errors in a string of \R code.
#'
#' @param string A character vector (of length one).
#' @export
validate_syntax <- function(string) {
  .Call("sourcetools_validate_syntax", string, PACKAGE = "sourcetools")
}

#' @export
print.RTokens <- function(x, ...) {
  result <- do.call(base::rbind, x)
  print(result)
}

#' @export
parse_string <- function(string) {
  .Call("sourcetools_parse_string", string, PACKAGE = "sourcetools")
}

#' @export
parse_file <- function(file) {
  parse_string(read(file))
}
