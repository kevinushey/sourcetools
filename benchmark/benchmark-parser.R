library(sourcetools)
library(microbenchmark)

files <- list.files("R", full.names = TRUE)
for (file in files) {

  mb <- microbenchmark(
    R  = base::parse(file, keep.source = FALSE),
    ST = sourcetools:::parse_file(file)
  )

  print(mb)

  contents <- sourcetools:::read(file)

  mb <- microbenchmark(
    R  = base::parse(text = contents, keep.source = FALSE),
    ST = sourcetools:::parse_string(contents)
  )

  sourcetools:::check_parse(contents)

}
