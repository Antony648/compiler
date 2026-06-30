# mini c compiler
a minimal aot compiler for c subset,the project implements lexer,parser,typechecker, and code generator from scratch.
# suppored features
it handles only int data type basic operations:
decclaration of vars
initilization of vars
assignment, 
basic arithemetic +,-,/,*
boolean comparision lessthan,greaterthan,lessthanequal,greaterthanequal
equal to ,notequal to
functions: cecdl convention function calls
if
while 
for
# limitations
function calls can not yet be incorporated to expressions,
so we cannot use return value of function calls to vars yet.. I did not thought about in while making the parser, but will implement it later

no support for float, char, or pointers

generates clumsly,unoptimized nasm output,this is due to lack of IR

# project contents and structure
main files include:

`compiler`: file that takes the input and starts the compilation and gel between different phases.
`lexer.c`: file that tokenizes the source
`lexer.h`: contains all structs and macros used in `lexer.c`

`parser.c`: builds the Abstract Syntax Tree,shunting yard infix to postifix followed by AST conversion
`parser.h`: contains structs and definition used in  `parser.c`

`symbol_table.h`: contains the struct of nodes in symbol table,created as separate file because we need it as part of `parser.h` as well as `typechecker.c`,including each other results in conflict

`typechecker.c`: validates scope and sematic rules,generates reversed singly-linked list referenced based counter
`typechecker.h`: contains structs and definitions used in `typechecker.c`
`codegen.c`: generates unoptimized asm 
`codegen.h`: support structs for `codegen.c`

`pretty_print.c`: prints contents of AST tree,for debugging purposes
`pretty_print.h`: header file for `pretty_print.c`

`Makefile`: build script for the whold project

# build
    ```bash 
make
```
# usage
    ```bash
./compiler <inputfile> <outputfile>
```
