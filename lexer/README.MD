# Lexer
This project is part one of a semester-long project building a compiler. My goal here is to build a lexer. 

# Running program
- Compile: g++ lexer.cpp token.cpp -o l
- To execute: ./l.exe filename.txt
- expected output: a series of lines written to the terminal of the form **Line <i> Char <j> <Token>**, where <i> and <j> denote the token's location in the file (both 1-indexed) <Token> is the token (and its value, if the token can have multiple values)

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
For this assignment, I have utilized the **Github Copilot** extention in VSCode, a tool created by **Github**. This uses the **GPT-4o** model created by **OpenAI**. I have used this tool for the following purposes:
- Automating repetitive tasks (i.e. filling all 20+ lines of a toString() function). This avoids me needing to write the same simple logic 20 times
- Splitting a token.cpp class into two files, token.h and token.cpp
- Writing extremely basic templates for certain functions
- Autocompleting certain lines of code, particularly in simple functions like Token::toString
- Generating the list of C++ keywords

# TODO
1. Add extra punctuation to lexer as needed