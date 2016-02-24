check_parse <- function(code) {
  if (!requireNamespace("testthat", quietly = TRUE))
    return(FALSE)

  R  <- base::parse(text = code, keep.source = FALSE)
  ST <- parse_string(code)
  if (length(R) == 1 && length(ST) == 1) {
    R  <- R[[1]]
    ST <- ST[[1]]
  }

  info <- paste(collapse = "\n", c(
    paste("R:  '", deparse(R), "'", sep = ""),
    paste("ST: '", deparse(ST), "'", sep = "")
  ))

  testthat::expect_equal(R, ST,
    base::parse(text = code)[[1]],
    parse_string(code)[[1]],
    info = info
  )

  TRUE
}
