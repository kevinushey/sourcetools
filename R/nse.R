#' Detect Whether a Function Performs Non-Standard Evaluation
#'
#' @param ... \R functions.
#' @export
performs_nse <- function(...) {
  .Call("sourcetools_performs_nse", list(...))
}
