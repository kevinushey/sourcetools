library(microbenchmark)
library(sourcetools)

file <- "R/sourcetools.R"
contents <- read(file)
repeat {
  tokenize_string(contents)
}

repeat {
  tokenize_file(file)
}
