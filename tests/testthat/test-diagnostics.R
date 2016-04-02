context("Diagnostics")

expect_no_diagnostics <- function(string) {
  diagnostics <- diagnose_string(string)
  expect_true(length(diagnostics) == 0)
  if (interactive()) print(diagnostics)
}

expect_diagnostics <- function(string) {
  diagnostics <- diagnose_string(string)
  expect_true(length(diagnostics) > 0)
  if (interactive()) print(diagnostics)
}

test_that("missing symbols reported appropriately", {
  expect_diagnostics("foo <- function(apple) { print(Apple) }")
})

test_that("unused computations are reported", {
  expect_diagnostics("foo <- function(apple) { apple < 1; print(TRUE) }")
})

test_that("use of '=' in if statement is reported", {
  expect_diagnostics("if (foo = 1) { print(1) }")
})

test_that("use of '&', '|' in 'if' is reported", {
  expect_diagnostics("if (1 & 2) print(1)")
  expect_diagnostics("if (1 | 2) print(1)")
})

test_that("x == NULL is reported", {
  expect_diagnostics("status <- print(1) == NULL; print(status)")
})
