files <- list.files("R", full.names = TRUE)
for (file in files) {

  mb <- microbenchmark::microbenchmark(
    R  = parse(file, keep.source = FALSE),
    ST = sourcetools:::parse(file)
  )

  print(mb)

  contents <- sourcetools:::read(file)

  mb <- microbenchmark::microbenchmark(
    R  = parse(text = contents, keep.source = FALSE),
    ST = sourcetools:::parse(text = contents)
  )

  sourcetools:::check_parse(contents)

}
