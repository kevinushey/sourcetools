context("Parser")

test_that("precedence of '?' vs '=' correctly handled", {

  skip_if(getRversion() < "4.0.3")

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

test_that("parser handles chained function calls", {
  expect_parse("a(b)(c)(d)(e)")
  expect_parse("a[b][c][d][e]")
  expect_parse("a[[b]][[c]][[d]][[e]]")
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
  # TODO: when deparsing UTF-8 escapes, Windows just prints
  # the code point and so this test fails. E.g.
  #
  #   > format("\u2665")
  #   [1] "<U+2665>"
  skip_on_os("windows")

  contents <- read("helper-utf8.R")
  expect_parse(contents)
})
