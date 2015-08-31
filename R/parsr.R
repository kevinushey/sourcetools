#' @useDynLib parsr
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
  .Call("parsr_read", path, PACKAGE = "parsr")
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
  .Call("parsr_tokenize_file", path, PACKAGE = "parsr")
}

#' Tokenize an R String
#'
#' Read and tokenize an R string.
#'
#' @param string A character vector (of length one).
#' @export
tokenize_string <- function(string) {
  .Call("parsr_tokenize_string", string, PACKAGE = "parsr")
}

#' Find Syntax Errors
#'
#' Find syntax errors in a string of \R code.
#'
#' @param string A character vector (of length one).
#' @export
validate_syntax <- function(string) {
  .Call("parsr_validate_syntax", string, PACKAGE = "parsr")
}

#' @export
print.RTokens <- function(x, ...) {
  result <- do.call(base::rbind, x)
  print(result)
}
