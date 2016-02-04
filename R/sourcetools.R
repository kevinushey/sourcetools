#' @useDynLib sourcetools
NULL

#' Read the Contents of a File
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

#' Tokenize R Code
#'
#' Tools for tokenizing \R code.
#'
#' @param path A file path.
#' @param string \R code as a character vector of length one.
#'
#' @return A \code{data.frame} with the following columns:
#'
#' \tabular{ll}{
#' \code{value}  \tab The token's contents, as a string.     \cr
#' \code{row}    \tab The row where the token is located.    \cr
#' \code{column} \tab The column where the token is located. \cr
#' \code{type}   \tab The token type, as a string.           \cr
#' }
#'
#' @rdname tokenize-methods
#' @export
#' @examples
#' tokenize_string("x <- 1 + 2")
tokenize_file <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call("sourcetools_tokenize_file", path, PACKAGE = "sourcetools")
}

#' @rdname tokenize-methods
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

# parse_string <- function(string) {
#   .Call("sourcetools_parse_string", string, PACKAGE = "sourcetools")
# }
#
# parse_file <- function(file) {
#   parse_string(read(file))
# }
