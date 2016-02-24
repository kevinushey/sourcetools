check_parse <- function(code) {
  if (!requireNamespace("testthat", quietly = TRUE))
    return(FALSE)

  testthat::expect_equal(
    parse(text = code)[[1]],
    parse_string(code)[[1]]
  )

  TRUE
}
