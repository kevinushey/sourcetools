diagnose_string <- function(string) {
  .Call("sourcetools_diagnose_string", as.character(string))
}

diagnose_file <- function(file) {
  diagnose_string(read(file))
}
