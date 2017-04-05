#!/usr/bin/env Rscript
knitr::render_markdown(strict = FALSE)
knitr::knit("README.Rmd", output = "README.md")
tools:::package_native_routine_registration_skeleton(".", "src/sourcetools-init.c")
