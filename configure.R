#!/usr/bin/env Rscript
knitr::render_markdown(strict = FALSE)
knitr::knit("README.Rmd", output = "README.md")
sourcetools:::register_routines(dynamic.symbols = TRUE)
