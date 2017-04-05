#' @useDynLib sourcetools
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
  .Call("sourcetools_read", path, PACKAGE = "sourcetools")
}

#' @name read
#' @rdname read
#' @export
read_lines <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call("sourcetools_read_lines", path, PACKAGE = "sourcetools")
}

#' @name read
#' @rdname read
#' @export
read_bytes <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call("sourcetools_read_bytes", path, PACKAGE = "sourcetools")
}

#' @name read
#' @rdname read
#' @export
read_lines_bytes <- function(path) {
  path <- normalizePath(path, mustWork = TRUE)
  .Call("sourcetools_read_lines_bytes", path, PACKAGE = "sourcetools")
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
  .Call("sourcetools_tokenize_file", path, PACKAGE = "sourcetools")
}

#' @rdname tokenize-methods
#' @export
tokenize_string <- function(string) {
  .Call("sourcetools_tokenize_string", as.character(string), PACKAGE = "sourcetools")
}

#' @rdname tokenize-methods
#' @export
tokenize <- function(file = "", text = NULL) {
  if (is.null(text))
    text <- read(file)
  tokenize_string(text)
}

#' @export
print.RTokens <- function(x, ...) {
  print.data.frame(x, ...)
}

