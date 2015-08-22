context("Tokenizer")

compare_tokens <- function(tokens, expected) {

  if (is.character(tokens))
    tokens <- tokenize_string(tokens)

  expect_true(
    length(tokens) == length(expected),
    "different number of tokens"
  )

  for (i in seq_along(tokens)) {
    expect_true(
      tokens[[i]][[1]] == expected[[i]],
      paste0("expected token '", tokens[[i]][[1]], "'; got '", expected[[i]], "'")
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

  for (operator in operators) {
    tokens <- tokenize_string(operator)
    expect_true(length(tokens) == 1, paste("expected a single token ('", operator, "')"))
  }
})

test_that("Numbers are tokenized correctly", {

  numbers <- c("1", "1.0", "0.1", ".1", "0.1E1", "1L", "1.0L", "1.5L",
               "1E1", "1E-1", "1E-1L", ".100E-105L")

  for (number in numbers) {
    tokens <- tokenize_string(number)
    expect_true(length(tokens) == 1, paste("expected a single token ('", number, "')"))
    token <- tokens[[1]]
    expect_true(token$type == "NUMBER", paste("expected a number ('", token$type, "')"))
  }

})

test_that("The tokenizer accepts UTF-8 symbols", {
  tokenize_string("å√∂")
  tokenize_string("¡™£¢∞§¶•ªº≠åß∂ƒ©˙∆˚¬…æΩ≈ç√∫˜µ≤≥÷")
})

test_that("The tokenizer works correctly", {

  # TODO: Should newlines be absorbed as part of the comment string?
  tokens <- tokenize_string("# A Comment\n")
  compare_tokens(tokens, "# A Comment\n")

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
