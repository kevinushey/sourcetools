context("Parser")

test_that("precedence of '?' vs '=' correctly handled", {

  expect_parse("foo ? bar = baz")
  expect_parse("foo ? bar <- baz")

})

test_that("parser handles simple control flow", {

  expect_parse("if (foo) bar + baz")
  expect_parse("while (1) 1 + 2")
  expect_parse("repeat 1 + 2")
  expect_parse("if (foo) bar else baz")
  expect_parse("if (foo) bar else if (baz) bat")
  expect_parse("for (i in 1:10) 1 + 10")

})

test_that("parser handles compound expressions", {

  expect_parse("if (foo) while (bar) 1")
  expect_parse("if (foo) (1 + 2)")
  expect_parse("{1; 2; 3}")
  expect_parse("{1 + 2\n3 + 4\n5 + 6}")

})

test_that("parser handles function calls", {
  expect_parse("foo <- bar(baz)[[1]]$bat")
  expect_parse("foo <- bar() + bam() * bat()")
})

test_that("parser handles precedence", {
  expect_parse("a$b[[1]]$c")
  expect_parse("object <- unclass(object)[i]")
})

test_that("parser handles numbers of various forms", {
  expect_parse(".15")
  expect_parse("15.")
  expect_parse("1.5")
  # expect_parse("1.5L") #TODO: R warns and parses as numeric
  expect_parse("15L")
  expect_parse("10E5")
  expect_parse("10E5L")
})

test_that("parser handles function calls with no args", {
  # Did you know?
  #
  # > length(base::parse(text = "a[]")[[1]])   # [1] 3
  # > length(base::parse(text = "a[[]]")[[1]]) # [1] 3
  #
  # R inserts an empty 'R_MissingArg' argument
  # into the third spot. This is sensible, albeit
  # a bit surprising when you first see it.
  expect_parse("a()")
  expect_parse("a[]")
  expect_parse("a[[]]")
})

test_that("parser recovers from missing commas", {
  expect_warning(expect_parse("a(1, 2, 3)", "a(1 2 3)"))
  expect_warning(expect_parse("function(a, b, c) 1", "function(a b c) 1"))
})

test_that("parser handles missing arguments", {
  expect_parse("a(,)")
  expect_parse("a[,]")
  expect_parse("a[[,]]")

  expect_parse("a(1,)")
  expect_parse("a[1,]")
  expect_parse("a[[1,]]")

  expect_parse("a(,1)")
  expect_parse("a[,1]")
  expect_parse("a[[,1]]")

  expect_parse("a(x =, b =)")
  expect_parse("quote(expr =)")
  expect_parse("a(x = ,)")
})

test_that("parser handles newlines as statement delimiter", {
  expect_parse("a <- b\n+1")
  expect_parse("a <- 1\n(b)")
  expect_parse("a <- foo(1)\n(b)")
  expect_parse("(a <- foo(1)\n(b))")
})

test_that("parser handles semi-colons as statement delimiter", {
  expect_parse("a <- 1; b <- 2; c <- 3")
  expect_parse("{a <- 1;}")
  expect_parse("{a <- 1;;; b}")
})

test_that("parser handles various escapes in strings", {
  expect_parse("'a = \\u{A0}'")
  expect_parse("a <- ifelse(a, '\\u{A0}', '\\u{A1}')")
})

test_that("parser normalizes string names in function calls", {
  expect_parse('"["(unclass(object), i)')
  expect_parse('"lol"(1, 2)')
})

test_that("parser handles if-else", {

  expect_parse("if (foo) {\nbar\n} else if (baz) {\n}")

})

test_that("parser handles various escapes in strings", {
  contents <- read("helper-utf8.R")
  expect_parse(contents)
})

test_that("parser handles random R code in my git folder", {
  skip("TODO")

  folders <- file.path(
    list.files("~/git", full.names = TRUE),
    "R"
  )

  files <- list.files(folders, full.names = TRUE, pattern = "[rR]$")

  for (i in seq_along(files)) {

    file <- files[[i]]
    contents <- read(file)
    cat("Checking parse: '", file, "'\n", sep = "")
    R <- base::parse(file, keep.source = FALSE)
    S <- sourcetools:::parse_string(contents)

    # The following is just for easier debugging
    dR <- deparse(R)
    dS <- deparse(S)

    bad <- dR != dS
    first <- which(bad)[1]
    rbind(R = dR[first], S = dS[first])
    rbind(R = dR[bad], S = dS[bad])

    # The actual test
    expect_identical(R, S)
  }

})

# generate test cases
if (FALSE) {

  names <- c("foo", "'bar'", "baz")
  unOps <- c("~", "+", "-", "?", "!")
  binOps <- c(
    # "::", ":::", ## TODO: only allowed in certain contexts
    # "$", "@", ## TODO: need to ensure following token is string or symbol
    "^", ":", "%foo%",
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

  n <- 3
  repeat {

    namesSample  <- sample(names, n, TRUE)
    binOpsSample <- sample(binOps, n, TRUE)
    unOpsSample <- unlist(lapply(seq_len(n), function(i) {
      paste(sample(unOps, n, TRUE), collapse = "")
    }))

    code <- paste(
      paste(unOpsSample, namesSample, binOpsSample, sep = " ", collapse = " "),
      "end"
    )

    R <- base::parse(text = code, keep.source = FALSE)
    S <- parse_string(code)
    stopifnot(all.equal(R, S))

  }

}
