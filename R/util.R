search_objects <- function() {
  lapply(seq_along(search()), function(i) {
    ls(pos = i, all.names = TRUE)
  })
}
