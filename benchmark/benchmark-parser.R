library(sourcetools)
library(microbenchmark)

files <- list.files("R", full.names = TRUE)
for (file in files) {

  mb <- microbenchmark(
    R  = base::parse(file, keep.source = FALSE),
    ST = sourcetools:::parse(file)
  )

  print(mb)

  contents <- sourcetools:::read(file)

  mb <- microbenchmark(
    R  = base::parse(text = contents, keep.source = FALSE),
    ST = sourcetools:::parse(text = contents)
  )

  sourcetools:::check_parse(contents)

}
