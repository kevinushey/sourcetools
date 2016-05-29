.sourcetools <- new.env(parent = emptyenv())
.sourcetools$gctorture <- TRUE

with_gctorture <- function(expr) {
  gctorture(.sourcetools$gctorture)
  result <- expr
  gctorture(FALSE)
  result
}

check_parse <- function(R, S = R) {
  lhs <- base::parse(text = R, keep.source = FALSE)
  rhs <- with_gctorture(parse_string(S))
  check_parse_impl(lhs, rhs)
}

check_parse_impl <- function(lhs, rhs) {

  lhsType <- typeof(lhs)
  rhsType <- typeof(rhs)

  onError <- function(format, ...) {
    message <- c(
      sprintf(format, ...),
      sprintf("R: '%s'", deparse(lhs)),
      sprintf("S: '%s'", deparse(rhs))
    )
    stop(paste(message, collapse = "\n"), call. = FALSE)
  }

  if (lhsType != rhsType)
    onError("TypeError: '%s' != '%s'", lhsType, rhsType)

  if (length(lhs) != length(rhs))
    onError("LengthError: %s != %s", length(lhs), length(rhs))

  if (is.call(lhs) || is.expression(lhs)) {
    lapply(seq_along(lhs), function(i) {
      check_parse_impl(lhs[[i]], rhs[[i]])
    })
  }

  if (!identical(lhs, rhs))
    onError("IdenticalError: '%s' != '%s'", lhs, rhs)

  TRUE
}

expect_parse <- function(R, S = R) {
  testthat::expect_true(check_parse(R, S))
}

search_objects <- function() {
  lapply(seq_along(search()), function(i) {
    ls(pos = i, all.names = TRUE)
  })
}
