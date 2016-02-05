context("Syntax")

test_that("no syntax errors are detected in R code in packages", {
  skip_on_cran()

  packages <- list.files("~/git", full.names = TRUE)
  rFolders <- file.path(packages, "R")
  rFiles <- unlist(lapply(rFolders, list.files, full.names = TRUE, pattern = "[rR]$"))
  for (file in rFiles) {
    contents <- readChar(file, nchars = file.info(file)$size, useBytes = TRUE)
    tokens <- tokenize_string(contents)
    errors <- validate_syntax(contents)
    expect_true(nrow(errors) == 0)
  }

})
