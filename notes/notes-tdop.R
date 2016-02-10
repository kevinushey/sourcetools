## An adapation of http://effbot.org/zone/simple-top-down-parsing.htm
## for R. We examine a super simple language (calculator)
## that consists only of '+', '*', and single-digit numbers
## (no whitespace). For example, "1+2*3+4" is a valid
## program, doing what you expect. More comments are
## included inline to help make sense of what's going on.
##
## While simple, this example showcases the main points
## needed to understand top-down operator precedence
## parsing.

## First, some simple utility functions.

# Check whether a token is an operator (ie, '+' or '*')
is_operator <- function(token) {
  token == "+" || token == "*"
}

# Single-digit numbers
is_number <- function(token) {
  token %in% as.character(0:9)
}

# We 'tokenize' a program (string of code) just by splitting
# it. So "1+2" becomes c("1", "+", "2"). Obviously a 'real'
# tokenizer would tokenize incrementally and separate words
# etc. but tokenization is not the interesting part of this
# example, so we just keep it simple.
tokenize <- function(program) {
  strsplit(program, "", fixed = TRUE)[[1]]
}

# A simple tokenizer 'class' that accepts a program,
# tokenizes it, and returns a method that accesses the next
# token (if available). Elements postfixed with '_' are
# 'private' (hidden in the closure); access to them is made
# available through 'public' functions exported as part of
# the list object.
Tokenizer <- function(program) {
  tokens_ <- tokenize(program)
  index_ <- 0
  n_ <- length(tokens_)
  list(
    tokenize = function() {
      index_ <<- index_ + 1
      if (index_ <= n_)
        tokens_[[index_]]
      else
        ""
    }
  )
}

# Our 'Parser' class will be used to construct
# our parse tree (an AST).
Parser <- function(tokenizer) {

  tokenizer_ <- tokenizer

  # We save a lookahead token, to help inform what action we
  # should take as we parse. It needs to exist as a private
  # variable so that the various recursing functions see the
  # correct 'state' of the program.
  lookahead_ <- tokenizer_$tokenize()

  # A hacky helper function for printing debug output when
  # running our parser. Don't worry too much about this.
  indent <- function() {
    paste(
      paste(character(length(sys.calls())), collapse = "-"),
      "-> ",
      sep = ""
    )
  }

  # The left-binding precedence for a token. The important
  # thing is that '*' has a higher precedence than '+'. This
  # function either receives operators, or a special 'end of
  # line' token, implying that there is nothing left to
  # parse. We give it a left-binding precedence of 0, to
  # indicate that parsing should end now.
  precedence <- function(token) {
    if (token == "+")
      10
    else if (token == "*")
      20
    else if (token == "")
      0
    else
      stop("unexpected token '", token, "'; expected operator or end-of-parse")
  }

  # Handling of 'null denotation' tokens. This is for tokens
  # that are discovered at the start of an expression; ie,
  # unary operators, or regular old numbers. Note how for
  # the '+' operator, we simply construct a single-element
  # node with "+" on the left-hand side, and the new
  # expression on the right-hand side.
  parsePrefixExpression <- function(token) {
    if (token == "+")
      call(token, parseTopLevelExpression(100))
    else if (is_number(token))
      as.numeric(token)
    else
      stop("unexpected token '", token, "'")
  }

  # 'led', for 'left denotation', is used when a token
  # appears within a construct (ie, when a binary operator
  # is encountered). This function will be called once a
  # binary operator is encountered, with 'lhs' being that
  # operator, and 'rhs' being the current parse tree. Each
  # call to 'led' constructs a new node, with our 'lhs'
  # operator as the parent, the current parse tree ('rhs')
  # as the left child, and the next part of the expression
  # as the right child.
  parseInfixExpression <- function(lhs, rhs) {
    if (!is_operator(lhs))
      stop("unexpected token '", lhs, "'; expecting an operator")
    call(lhs, rhs, parseTopLevelExpression(precedence(lhs)))
  }

  # This is the entry-point that parses a whole expression.
  parseTopLevelExpression <- function(rbp = 0) {

    # Save the current token in 'token', and advance to the
    # next token.
    #
    # Why do we need to save the token in a 'global'
    # variable? When the various parse recursions end, we
    # need to make sure those routines are seeing the
    # current state, rather than their own state.
    token <- lookahead_
    lookahead_ <<- tokenizer_$tokenize()

    # Parse the 'null denotation' expression. This
    # represents tokens that are discovered at the beginning
    # of an expression. We expect this to handle both unary
    # operators (wherein 'nud' will recurse until
    # discovering a non-operator token), and numeric tokens
    # (which end the recursion).
    cat(indent(), "lhs <- parsePrefixExpression(", format(token), ")\n", sep = "")
    node <- parsePrefixExpression(token)

    # Now, we need to construct the right-hand side of this
    # expression. The 'lbp' tells us whether we can continue
    # 'joining' expressions into the current parse tree.
    # TODO: make this more clear
    while (rbp < precedence(lookahead_)) {

      # Save the current token, and get the next token.
      token <- lookahead_
      lookahead_ <<- tokenizer$tokenize()

      # Construct a new 'node' for our tree. Notice how we
      # 'grow' the left-hand side here.
      cat(indent(), "lhs <- parseInfixExpression(", format(token), ", ", format(node), ")\n", sep = "")
      node <- parseInfixExpression(token, node)
    }

    # Return our parse tree.
    node
  }

  list(parse = parseTopLevelExpression)
}

# Let's test it out!
program <- "1+2*3*4+5"
tokens <- tokenize(program)
tokenizer <- Tokenizer(program)
parser <- Parser(tokenizer)
expr <- parser$parse()
stopifnot(eval(parse(text = program)) == eval(expr))
