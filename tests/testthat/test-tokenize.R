context("Tokenizer")

compare_tokens <- function(tokens, expected) {

  if (is.character(tokens))
    tokens <- tokenize_string(tokens)

  expect_true(
    nrow(tokens) == length(expected),
    "different number of tokens"
  )

  for (i in 1:nrow(tokens)) {
    expect_true(
      tokens$value[[i]] == expected[[i]],
      paste0("expected token '", tokens$value[[i]], "'; got '", expected[[i]], "'")
    )
  }

}

test_that("Operators are tokenized correctly", {

  operators <- c(
    "::", ":::", "$", "@", "[", "[[", "^", "-", "+", ":",
    "*", "/", "+", "-", "<", ">", "<=", ">=", "==", "!=",
    "!", "&", "&&", "|", "||", "~", "->", "->>", "<-", "<<-",
    "=", "?", "**", "%%", "%for%"
  )

  tokenized <- tokenize_string(paste(operators, collapse = " "))

  for (operator in operators) {
    tokens <- tokenize_string(operator)
    expect_true(nrow(tokens) == 1, paste("expected a single token ('", operator, "')"))
  }
})

test_that("Numbers are tokenized correctly", {

  numbers <- c("1", "1.0", "0.1", ".1", "0.1E1", "1L", "1.0L", "1.5L",
               "1E1", "1E-1", "1E-1L", ".100E-105L", "0.", "100.",
               "1e+09", "1e+90", "1e-90", "1e-00000000000000009")

  for (number in numbers) {
    tokens <- tokenize_string(number)
    expect_true(nrow(tokens) == 1, paste("expected a single token ('", number, "')", sep = ""))
    token <- as.list(tokens[1, ])
    expect_true(token$type == "number", paste("expected a number ('", token$type, "')", sep = ""))
  }

})

test_that("The tokenizer accepts UTF-8 symbols", {
  expect_true(nrow(tokenize_string("鬼")) == 1)
})

test_that("The tokenizer works correctly", {

  # TODO: Should newlines be absorbed as part of the comment string?
  tokens <- tokenize_string("# A Comment\n")
  expected <- "# A Comment\n"
  compare_tokens(tokens, expected)

  tokens <- tokenize_string("a <- 1 + 2\n")
  compare_tokens(
    tokens,
    c("a", " ", "<-", " ", "1", " ", "+", " ", "2", "\n")
  )

  compare_tokens(
    tokenize_string("a<-1"),
    c("a", "<-", "1")
  )

  # NOTE: '-' sign tokenized separately from number
  compare_tokens(
    tokenize_string("a< -1"),
    c("a", "<", " ", "-", "1")
  )

  compare_tokens("1.0E5L", "1.0E5L")
  compare_tokens(".1", ".1")
  compare_tokens("'\\''", "'\\''")
  compare_tokens(".a", ".a")
  compare_tokens("...", "...")
  compare_tokens(":=", ":=")
  compare_tokens("x ** 2", c("x", " ", "**", " ", "2"))

})

test_that("`[[` and `[` are tokenized correctly", {

  compare_tokens("x[[1]]", c("x", "[[", "1", "]]"))

  # not really valid R code, but the tokenizer should still
  # get it right
  compare_tokens("[[[]]]", c("[[", "[", "]", "]]"))

  compare_tokens(
    "x[[a[b[[c[1]]]]]]",
    c("x", "[[", "a", "[", "b", "[[", "c", "[", "1",
      "]", "]]", "]", "]]")
  )

})

test_that("Failures during number tokenization is detected", {
  tokens <- tokenize_string("1.5E---")
  expect_true(tokens$type[[1]] == "invalid")
})

test_that("invalid number e.g. 1E1.5 tokenized as single entity", {
  tokens <- tokenize_string("1E1.5")
  expect_true(nrow(tokens) == 1)
  expect_true(tokens$type[[1]] == "invalid")
})

test_that("keywords are tokenized as keywords", {

  keywords <- c("if", "else", "repeat", "while", "function",
                "for", "in", "next", "break",
                "TRUE", "FALSE", "NULL", "Inf", "NaN", "NA",
                "NA_integer_", "NA_real_", "NA_complex_", "NA_character_")

  tokens <- lapply(keywords, function(keyword) {
    tokenize_string(keyword)[1, ]
  })

  types <- unlist(lapply(tokens, `[[`, "type"))
  expect_true(all(types == "keyword"))
})

test_that("comments without a trailing newline are tokenized", {
  tokens <- tokenize_string("# abc")
  expect_identical(tokens$type, "comment")
})

test_that("tokenization errors handled correctly", {
  # previously, these reported an error where a NUL
  # byte was accidentally included as part of the
  # token value
  tokenize_string("`abc")
  tokenize_string("'abc")
  tokenize_string("\"abc")
  tokenize_string("%abc")
})

test_that("files in packages are tokenized without errors", {
  skip_on_cran()

  paths <- list.dirs("~/git", full.names = TRUE, recursive = FALSE)
  packages <- paths[file.exists(file.path(paths, "DESCRIPTION"))]
  R <- file.path(packages, "R")

  for (dir in R) {
    files <- list.files(dir, pattern = "R$", full.names = TRUE)
    for (file in files) {
      tokens <- tokenize_file(file)
      errors <- tokens$type == "invalid"
      expect_true(all(errors == FALSE))
    }
  }

})
