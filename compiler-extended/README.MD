# Compiler
This is my submission for the compiler project extension. This is the most up-to-date portion of the project.

# Current Progress
I have not yet completed my compiler. Currently, I have a functioning lexer and a mostly-functioning parser. A function is created for each rule, but there are some outlying problems remaining. Currently, the AST is built successfully, and I am working on the code generation portion. I 

# Running program
- Compile: g++ lexer.cpp ast.cpp token.cpp -o c
- To execute: ./c.exe filename.txt
- expected input: a file containing a program which is valid in this grammar
- expected output: a series of lines written to a file called "output.txt". This is valid stack machine code. 

# Useful Supplementary Files
- LT_Compiler_Grammar.docs: A document defining my compiler
- example.txt: An example input file 
- first_follows.txt: A set f the firsts and follows for each identifer

# Language
The language is defined with the following specification. An example file was provided above.

"Develop a complete compiler that produces stack machinecode for the liittle language of your grammar. You language must resemble B  or the C minus minus I provided. It must support integers and have functions(passing arguments, returning values), variables,expressions, iteration, selection and assignment."


# Language specification
This lexer supports int, string, and char variable types. It reserves the following keywords:
- **main**, **std** (main)
- **int**, **string**, **char**, **bool** (variable types)
- **if**, **else**, **while**, **for** (control flow)
- **return**, **break**, **continue** (loop control)
- **true**, **false** (boolean literals)
- **void**, **const** (type qualifiers)
- **class**, **struct** (user-defined types)
- **public**, **private**, **protected** (access specifiers)
- **new**, **delete** (memory management)
- **try**, **catch**, **throw** (exception handling)
- **namespace**, **using** (namespaces)
- **sizeof**, **typeid** (type information)
- **static**, **extern** (storage duration)
- **switch**, **case**, **default** (switch statements)
- **do**, **while** (loop constructs)
- **operator**, **friend** (operator overloading and friend functions)
- **this**, **nullptr** (pointers and references)
- **inline**, **virtual**, **override** (function specifiers)
- **template**, **typename** (templates)
- **enum**, **union** (enumerations and unions)
- **goto** (jump statement)
- **auto**, **register**, **volatile**, **mutable**, **explicit**, **typedef**, **asm**, **__asm**, **__asm__**, **__volatile__**, **__volatile**, **__volatile**, **__volatile**, **__volatile**, **__volatile**, **__volatile** (other)

# AI Statement
For this assignment, I have utilized the **Github Copilot** extention in VSCode, a tool created by **Github**. This uses the **GPT-4o** model created by **OpenAI**. I also used the Claude AI model. I have used this tool for the following purposes:
- Automating repetitive tasks (i.e. filling all 20+ lines of a toString() function). This avoids me needing to write the same simple logic 20 times
- Splitting a token.cpp class into two files, token.h and token.cpp
- Splitting an ast.cpp file into two files, ast.h and token.h
- Writing extremely basic templates for certain functions
- Autocompleting certain lines of code, particularly in simple functions like Token::toString
- Generating the list of C++ keywords
- Miscellaneous debugging help