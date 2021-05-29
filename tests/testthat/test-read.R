context("Reader")

files <- list.files(
  pattern = "[.]R$",
  full.names = TRUE,
  include.dirs = FALSE
)

test_that("read_lines and readLines agree on output", {
  for (file in files) {
    expect_identical(
      readLines(file, warn = FALSE),
      sourcetools::read_lines(file)
    )
  }
})

test_that("read and readChar agree on output", {
  for (file in files) {
    expect_identical(
      readChar(file, file.info(file)$size, TRUE),
      sourcetools::read(file)
    )
  }
})

test_that("read_bytes and readBin agree on output", {
  for (file in files) {
    expect_identical(
      readBin(file, "raw", file.info(file)$size),
      sourcetools::read_bytes(file)
    )
  }
})

test_that("read_lines can handle '\r' line endings", {
  
  file <- tempfile()
  on.exit(unlink(file), add = TRUE)
  
  text <- "this\ris\rsome\rtext"
  writeLines(text, con = file, useBytes = TRUE)
  
  r <- readLines(file)
  s <- read_lines(file)
  expect_identical(r, s)
  
})

test_that("read_lines can handle '\r\n' line endings", {
  
  file <- tempfile()
  on.exit(unlink(file), add = TRUE)
  
  text <- "this\r\nis\r\nsome\r\ntext\r"
  writeLines(text, con = file, useBytes = TRUE)
  
  r <- readLines(file)
  s <- read_lines(file)
  expect_identical(r, s)
  
})

test_that("read_lines can handle mixed line endings", {
  
  file <- tempfile()
  on.exit(unlink(file), add = TRUE)
  
  text <- "this\ris\nsome\r\ntext\r"
  writeLines(text, con = file, useBytes = TRUE)
  
  r <- readLines(file)
  s <- read_lines(file)
  expect_identical(r, s)
  
})
