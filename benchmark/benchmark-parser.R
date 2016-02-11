path <- "R/sourcetools.R"
contents <- read(path)

library(microbenchmark)
parse_string <- sourcetools:::parse_string

microbenchmark(
  R  = parse(text = contents),
  ST = parse_string(contents)
)
