# convert a file path to a header guard name
guarded_name <- function(path) {

  transformations <- list(
    dots    = function(x) gsub(".", "_", x, fixed = TRUE),
    camel   = function(x) gsub("(?<!^)([A-Z])", "_\\1", x, perl = TRUE),
    upper   = function(x) toupper(x)
  )

  parts <- strsplit(path, "/", fixed = TRUE)[[1]]
  paste(lapply(parts, function(part) {
    transformed <- part
    for (f in transformations)
      transformed <- f(transformed)
    transformed
  }), collapse = "__")

}

update_headers <- function() {

  # move to include directory
  owd <- setwd("inst/include")
  on.exit(setwd(owd), add = TRUE)

  files <- list.files(
    "sourcetools",
    pattern = "h(?:pp)?$",
    recursive = TRUE,
    full.names = TRUE
  )

  for (file in files) {
    guard <- guarded_name(file)
    contents <- sourcetools::read_lines(file)
    n <- length(contents)
    contents[[1]] <- paste("#ifndef", guard)
    contents[[2]] <- paste("#define", guard)
    contents[[n]] <- sprintf("#endif /* %s */", guard)
    cat(contents, file = file, sep = "\n")
  }
}

update_headers()
