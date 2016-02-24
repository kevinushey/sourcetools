context("Parser")

test_that("agreement on parse of expressions", {

  check_parse("foo ? bar = baz")
  check_parse("foo ? bar <- baz")

})

test_that("parser handles simple control flow", {

  check_parse("if (foo) bar + baz")
  check_parse("while (1) 1 + 2")
  check_parse("repeat 1 + 2")
  check_parse("if (foo) bar else baz")
  check_parse("if (foo) bar else if (baz) bat")
  check_parse("for (i in 1:10) 1 + 10")

})

test_that("parser handles compound expressions", {

  check_parse("if (foo) while (bar) 1")
  check_parse("if (foo) (1 + 2)")
  check_parse("{1; 2; 3}")
  check_parse("{1 + 2\n3 + 4\n5 + 6}")

})

test_that("parser handles function calls", {
  check_parse("foo <- bar(baz)[[1]]$bat")
  check_parse("foo <- bar() + bam() * bat()")
})

test_that("parser handles precedence", {
  check_parse("a$b[[1]]$c")
})

test_that("parser handles random R code in my git folder", {
  skip("TODO")

  folders <- file.path(
    list.files("~/git", full.names = TRUE),
    "R"
  )

  files <- list.files(folders, full.names = TRUE)

  for (file in files) {
    contents <- read(file)
    cat("Checking parse: '", file, "'\n", sep = "")
    R  <- base::parse(text = contents, keep.source = FALSE)
    ST <- sourcetools:::parse_string(contents)
    expect_true(all.equal(R, ST))
    Sys.sleep(1)
  }

})

# generate test cases
if (FALSE) {

  names <- c("foo", "'bar'", "baz")
  unOps <- c("~", "+", "-", "?", "!")
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
    "=",
    "?"
  )

  n <- 5
  repeat {
    code <- paste(
      paste(sample(unOps, n), collapse = ""),
      paste(sample(names, n, TRUE), sample(binOps, n, TRUE), collapse = " "),
      " end",
      sep = ""
    )

    R <- parse(text = code)
    attributes(R) <- NULL
    ST <- parse_string(code)
    stopifnot(all.equal(R, ST))
  }

}
