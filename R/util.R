check_parse <- function(...) {
  vapply(list(...), FUN.VALUE = logical(1), function(code) {
    all.equal(
      parse(text = code)[[1]],
      parse_string(code)[[1]]
    )
  })
}
