

sourcetools
===========

Tools for tokenizing and (eventually) parsing R code.

## Getting Started

`sourcetools` is not yet on CRAN -- install with


{% highlight r %}
devtools::install_github("kevinushey/sourcetools")
{% endhighlight %}

## Tokenization

`sourcetools` provides the `tokenize_string()` and
`tokenize_file()` functions for generating a tokenized
repreesntation of R code. These produce 'raw' tokenized
representations of the code, with each token's value as a
string, and a recorded row, column, and type:


{% highlight r %}
tokenize_string("if (x < 10) 20")
{% endhighlight %}



{% highlight text %}
##    value row column       type
## 1     if   1      1    keyword
## 2          1      3 whitespace
## 3      (   1      4    bracket
## 4      x   1      5     symbol
## 5          1      6 whitespace
## 6      <   1      7   operator
## 7          1      8 whitespace
## 8     10   1      9     number
## 9      )   1     11    bracket
## 10         1     12 whitespace
## 11    20   1     13     number
{% endhighlight %}

## Syntax Validation

`validate_syntax()` is provided to identify syntax errors:


{% highlight r %}
validate_syntax("{1 + 2)")
{% endhighlight %}



{% highlight text %}
##   row column                error
## 1   0      6 unexpected token ')'
{% endhighlight %}
