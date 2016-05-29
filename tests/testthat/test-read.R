context("Reader")

files <- list.files()

test_that("read_lines and readLines agree on output", {
  for (file in files) {
    expect_identical(
      readLines(file),
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
