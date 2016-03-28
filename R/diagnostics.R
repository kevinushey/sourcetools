diagnose_file <- function(file) {
  diagnostics <- .Call("sourcetools_diagnose_file", file)
}
