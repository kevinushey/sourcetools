Parser
======

- `::` and `:::` are only permitted within certain contexts; the parser is currently permissive about where these tokens are found.

- Equality operators (`<`, `<=`, `>`, `>=`, `=`, `!=`) can only occur once within the same level of an expression.

- `->` and `->>` need to be translated into `<-` and `<<-` when generating the R parse tree.



Syntax Validator
================

Remove it? It really just tries to check for parse errors but the parser itself is equipped to do that.



