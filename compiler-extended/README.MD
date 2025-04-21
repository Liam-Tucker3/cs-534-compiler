# Compiler
This is my submission for the compiler project extension. This is the most up-to-date portion of the project. (I understand that generally branches or version history would be used here, but this is the easiest and most explicit way to avoid the wrong thing being graded.)

# Current Progress
The compiler is fully functioning.

# To run file
- The command **make** will compile the compiler and create an executable called c.exe
- The command **make stack** will compile the stack machine and create an executable called s.exe
- To compile using my compiler, use the command: **./c.exe filename.txt**, where **filename.txt** is your program. This will produce a file called **compiled.txt**, which is the compiled output containing valid stack machine code. 
- To test the stack machine code, use the command **./s.exe compiled.txt**. The output will be printed to standard out.

# Files in this directory
- **documentation/LT_Compiler_Grammar.docx**: a document defining the language used in generating my Abstract Syntax Tree. Note that the Code Generator uses a slightly different set of rules. For instance, the Code Generator combines Rules 25 and 26 into one.
- **examples/**: a directory containing a handful of files used as inputs or outputs for tests. **gcd_template.txt** is the GCD stack machine code, adapted in form for my stack machine. **example.txt** is an example program in my language to calculate the GCD, and **compiled.txt** is the stack machine code emitted by my compiler.
- **ast.h** and **ast.cpp**: Defines the ASTNode, SymbolTable, and Parser classes used in my compiler.
- **codeGenerator.h** and **codeGenerator.cpp**: Defines the CodeGenerator for my compiler.
- **token.h** and **token.cpp**: Defines the Token class used in my compiler.
- **stackMachine.cpp**: Defines the stack machine that serves as the target language.
- **lexer.cpp**: Conntains code to perform lexing for my compiler. Also contains the main() function called by my compiler.
- **stackMachineMain.cpp**: Contains the main function for my stack machine

# Language
The language is defined with the following specification. An example file was provided above.

"Develop a complete compiler that produces stack machinecode for the liittle language of your grammar. You language must resemble B  or the C minus minus I provided. It must support integers and have functions(passing arguments, returning values), variables,expressions, iteration, selection and assignment."

# Valid Stack Machine Commands
- CALL(), CALL(std::string label)
- RET(), RETV()
- PUSH(), PUSH(int value), POP()
- DUP()
- LOAD(), SAVE(), STORE()
- ADD(), SUB(), MUL(), DIV(), REM()
- EQ(), NE(), LE(), GE(), LT(), GT()
- BRT(), BRT(std::string label), BRT(int loc), BRZ(), BRZ(std::string label), BRZ(int loc)
- JUMP(), JUMP(std::string label), JUMP(int loc)
- PRINT(), PRINT(std::string message)
- READ()
- END()

# Reserved keywords
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
- Generating the makefile
- Writing extremely basic templates for certain functions
- Autocompleting certain lines of code, particularly in simple functions like Token::toString
- Generating the list of C++ keywords
- Miscellaneous debugging help

I have reviewed all code which has appeared in this project and am comfortable accepting full ownership of all code here.