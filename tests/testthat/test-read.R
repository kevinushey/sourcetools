context("Reader")

files <- list.files(
  pattern = "[.]R$",
  full.names = TRUE,
  include.dirs = FALSE
)

test_that("read_lines and readLines agree on output", {
  for (file in files) {
    r <- readLines(file, warn = FALSE, encoding = "UTF-8")
    s <- sourcetools::read_lines(file)
    expect_identical(r, s)
  }
})

test_that("read and readChar agree on output", {
  for (file in files) {
    r <- readChar(file, file.info(file)$size, TRUE)
    Encoding(r) <- "UTF-8"
    s <- sourcetools::read(file)
    expect_identical(r, s)
  }
})

test_that("read_bytes and readBin agree on output", {
  for (file in files) {
    r <- readBin(file, "raw", file.info(file)$size)
    s <- sourcetools::read_bytes(file)
    expect_identical(r, s)
  }
})

test_that("read_lines can handle '\\r' line endings", {

  file <- tempfile()
  on.exit(unlink(file), add = TRUE)

  text <- "this\ris\rsome\rtext"
  writeLines(text, con = file, useBytes = TRUE)

  r <- readLines(file)
  s <- read_lines(file)
  expect_identical(r, s)

})

test_that("read_lines can handle '\\r\\n' line endings", {

  file <- tempfile()
  on.exit(unlink(file), add = TRUE)

  text <- "this\r\nis\r\nsome\r\ntext\r"
  writeBin(charToRaw(text), file)

  r <- readLines(file)
  s <- read_lines(file)
  expect_identical(r, s)

})

test_that("read_lines can handle mixed line endings", {

  file <- tempfile()
  on.exit(unlink(file), add = TRUE)

  text <- "this\ris\nsome\r\ntext\r"
  writeBin(charToRaw(text), file)

  r <- readLines(file)
  s <- read_lines(file)
  expect_identical(r, s)

})
