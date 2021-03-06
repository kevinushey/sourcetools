#' @useDynLib sourcetools, .registration = TRUE
NULL

#' Read the Contents of a File
#'
#' Read the contents of a file into a string (or, in the case of
#' \code{read_lines}, a vector of strings).
#'
#' @param path A file path.
#'
#' @name read
#' @rdname read
#' @export
read <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call(sourcetools_read, path)
}

#' @name read
#' @rdname read
#' @export
read_lines <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call(sourcetools_read_lines, path)
}

#' @name read
#' @rdname read
#' @export
read_bytes <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call(sourcetools_read_bytes, path)
}

#' @name read
#' @rdname read
#' @export
read_lines_bytes <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call(sourcetools_read_lines_bytes, path)
}

#' Tokenize R Code
#'
#' Tools for tokenizing \R code.
#'
#' @param file,path A file path.
#' @param text,string \R code as a character vector of length one.
#'
#' @note Line numbers are determined by existence of the \code{\\n}
#' line feed character, under the assumption that code being tokenized
#' will use either \code{\\n} to indicate newlines (as on modern
#' Unix systems), or \code{\\r\\n} as on Windows.
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
  .Call(sourcetools_tokenize_file, path)
}

#' @rdname tokenize-methods
#' @export
tokenize_string <- function(string) {
  .Call(sourcetools_tokenize_string, as.character(string))
}

#' @rdname tokenize-methods
#' @export
tokenize <- function(file = "", text = NULL) {
  if (is.null(text))
    text <- read(file)
  tokenize_string(text)
}

#' Find Syntax Errors
#'
#' Find syntax errors in a string of \R code.
#'
#' @param string A character vector (of length one).
#' @export
validate_syntax <- function(string) {
  .Call(sourcetools_validate_syntax, as.character(string))
}

#' @export
print.RTokens <- function(x, ...) {
  print.data.frame(x, ...)
}

parse_string <- function(string) {
  .Call(sourcetools_parse_string, string)
}

parse_file <- function(file) {
  parse_string(read(file))
}
