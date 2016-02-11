context("Parser")

test_that("agreement on parse of expressions", {

  check_parse("foo ? bar = baz") # R believes that '?' is higher precedence than '='?
  check_parse("foo ? bar <- baz")

})

# generate test cases
if (FALSE) {

  names <- c("foo", "'bar'", "baz")

  binOps <- c(
    # "::", ":::", ## TODO: only allowed in certain contexts
    "$", "@", "^", ":", "%foo%",
    "*", "/", "+", "-",
    # "<", ">", "<=", ">=", "==", "!=", ## TODO: The R parser checks these are only found once in expression
    "&", "&&",
    "|", "||",
    "~",
    # "->", "->>", ## TODO: The R parser translates these into `<-` assignments
    "<-", "<<-",
    "="
    "?"
  )

  n <- 3
  repeat {
    code <- paste(
      paste(sample(names, n, TRUE), sample(binOps, n, TRUE), collapse = " "),
      "end"
    )

    R <- parse(text = code)
    attributes(R) <- NULL
    ST <- parse_string(code)
    stopifnot(all.equal(R, ST))
  }

}
