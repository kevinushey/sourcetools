context("Diagnostics")

test_that("missing symbols reported appropriately", {
  parse_string("a <- 1; print(a)")
  parse_string("foo::bar")
})
