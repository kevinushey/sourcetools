code_complete_at <- function(file, position) {
  file <- normalizePath(file, mustWork = TRUE)
  position <- as.numeric(position)
  .Call("sourcetools_code_complete_at", file, position)
}
