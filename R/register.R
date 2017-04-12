#' Register Native Routines
#'
#' Discover and register native routines in a package.
#' Functions to be registered should be prefixed with the
#' `// [[export(<methods>)]]` attribute.
#'
#' @param package The path to an \R package.
#' @param prefix The prefix to assign to the \R objects
#'   generated that map to each routine.
#' @param dynamic.symbols Boolean; should dynamic symbol lookup
#'   be enabled?
#'
register_routines <- function(package = ".",
                              prefix = "C_",
                              dynamic.symbols = FALSE)
{
  # read DESCRIPTION file
  desc_path <- file.path(package, "DESCRIPTION")
  if (!file.exists(desc_path)) {
    fmt <- "no DESCRIPTION at path '%s'"
    stop(sprintf(fmt, desc_path))
  }
  desc <- read.dcf(desc_path, all = TRUE)
  pkg_name <- desc$Package

  # find C, C++ files in package
  srcfiles <- list.files(
    package,
    pattern = "\\.(?:h|c|cc|cpp)$",
    full.names = TRUE,
    recursive = TRUE
  )

  # discover routines in these files
  routines <- unlist(
    lapply(srcfiles, discover_routines),
    recursive = FALSE
  )

  # generate prototypes based on routines
  prototypes <- generate_prototypes(routines)

  # separate routines based on declared export type
  call_routines <- external_routines <- list()
  lapply(routines, function(routine) {

    # extract registration text and discover the interface
    pieces <- strsplit(routine$registration, "\\[\\[|\\]\\]")[[1]]
    code <- utils::tail(pieces, 1)
    parsed <- tryCatch(
      parse(text = code)[[1]],
      error = function(e) {
        warning("failed to parse registration comment '", routine$registration, "'")
        next
      }
    )

    interface <- as.character(parsed[[2]])
    if (interface == ".Call") {
      call_routines[[length(call_routines) + 1]] <<- routine
    } else if (interface == ".External") {
      external_routines[[length(external_routines) + 1]] <<- routine
    } else {
      warning("unrecognized / unsupported interface '", interface, "'")
      next
    }
  })

  # generate method definitions for each
  call_methods <- generate_call_methods(call_routines, prefix = prefix)
  external_methods <- generate_external_methods(external_routines, prefix = prefix)

  # generate initialization routine
  r_init <- generate_r_init(pkg_name = pkg_name,
                            call_methods = call_methods,
                            external_methods = external_methods,
                            dynamic_symbols = dynamic.symbols)

  # generate script
  script <- c(
    "// This file was automatically generated.",
    "",
    "#include <R.h>",
    "#include <Rinternals.h>",
    "#include <R_ext/Rdynload.h>",
    "",
    prototypes,
    "",
    call_methods,
    "",
    external_methods,
    "",
    r_init
  )

  # write to init file
  init_path <- sub("^\\./", "", file.path(package, sprintf("src/%s-init.c", pkg_name)))
  writeLines(script, init_path, sep = "\n")
  message("* Wrote registration metadata to '", init_path, "'")

  # remind about .registration = TRUE
  check_namespace_symbol_registration(package)
  invisible(init_path)
}

discover_routines <- function(file) {
  contents <- readBin(file, what = raw(), n = file.info(file)$size)

  # find routines for registration
  re_registration <- "//[[:space:]*]\\[\\[export"
  if (length(contents) < re_registration)
    return(list())

  matches <- grepRaw(re_registration, contents, all = TRUE)
  lapply(matches, function(match) {

    # find bounds for function prototype
    start <- grepRaw("\n",    contents, offset = match) + 1
    end   <- grepRaw("\\{|;", contents, offset = start) - 1

    # extract the routine type
    registration <- rawToChar(contents[match:(start - 2)])

    # extract all 'SEXP .*' pieces of function
    prototype <- rawToChar(contents[start:end])
    m <- gregexpr("SEXP[[:space:]+]([[:alnum:]_])+", prototype)
    names <- regmatches(prototype, m)[[1]]

    list(
      registration = registration,
      prototype = prototype,
      name = names[[1]],
      arguments = names[-1L]
    )

  })

}

check_namespace_symbol_registration <- function(package = ".") {

  # check for namespace file
  ns_path <- file.path(package, "NAMESPACE")
  if (!file.exists(ns_path))
    return(invisible(FALSE))

  # try parsing the namespace
  ns <- parse(ns_path)

  # try finding a call to 'useDynLib(pkg, .registration = TRUE)'
  for (entry in ns) {
    if (identical(entry[[1]], as.name("useDynLib"))) {
      nm <- names(entry)
      idx <- which(nm == ".registration")
      if (length(idx) != 1)
        break

      if (isTRUE(entry[[idx]]))
        return(invisible(TRUE))
    }
  }

  invisible(FALSE)
}

generate_prototypes <- function(routines) {
  # TODO: we assume only SEXP interfaces here
  vapply(routines, function(routine) {
    arglist <- paste(rep("SEXP", length(routine$arguments)), collapse = ", ")
    sprintf("%s(%s);", routine$name, arglist)
  }, character(1))
}

generate_call_methods <- function(routines, prefix = "C_") {

  # for each routine, generate a registration line
  fmt <- '{"%s", (DL_FUNC) &%s, %i},'
  lines <- vapply(routines, function(routine) {
    name <- utils::tail(strsplit(routine$name, "[[:space:]+]")[[1]], 1)
    prefixed_name <- paste0(prefix, name)
    n <- length(routine$arguments)
    sprintf(fmt, prefixed_name, name, n)
  }, character(1))

  # indent, add commas, add null entry at end
  lines <- c(lines, "{NULL, NULL, 0}")

  c(
    "static R_CallMethodDef callMethods[] = {",
    paste0("\t", lines),
    "};"
  )

}

generate_external_methods <- function(routines, prefix = "C_") {
  # TODO
  character()
}

generate_r_init <- function(pkg_name,
                            call_methods,
                            external_methods,
                            dynamic_symbols)
{
  r_register_routines <- sprintf(
    "\tR_registerRoutines(info, %s, %s, %s, %s);",
    "NULL",
    if (length(call_methods)) "callMethods" else "NULL",
    "NULL",
    if (length(external_methods)) "externalMethods" else "NULL"
  )

  fmt <- paste(
    "void R_init_%s(DllInfo* info) {",
    r_register_routines,
    "\tR_useDynamicSymbols(info, %s);",
    "}",
    sep = "\n", collapse = "\n"
  )

  sprintf(fmt, pkg_name, if (dynamic_symbols) "TRUE" else "FALSE")

}
