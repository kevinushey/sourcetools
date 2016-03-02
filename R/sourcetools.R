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
#' @note New rows are determined by existence of the \code{\\n}
#' line feed character, under the assumption that code being tokenized
#' will use either \code{\\n} to indicate newlines (as on modern
#' Linux and OS X systems), or \code{\\r\\n} as on Windows.
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
  .Call("sourcetools_tokenize_string", as.character(string), PACKAGE = "sourcetools")
}

#' @export
print.RTokens <- function(x, ...) {
  print.data.frame(x, ...)
}
