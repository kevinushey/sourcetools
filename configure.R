#!/usr/bin/env Rscript
knitr::render_jekyll()
knitr::knit("README.Rmd", output = "README.md")
