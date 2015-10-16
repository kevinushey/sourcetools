library(microbenchmark)

files <- list.files("~/git/dplyr/R/", full.names = TRUE)
lapply(files, function(file) {
  contents <- readChar(file, file.size(file), TRUE)
  microbenchmark(
    tokenize_string(contents),
    parse(text = contents),
    times = 10
  )
})
