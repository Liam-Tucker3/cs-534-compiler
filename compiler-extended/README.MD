# Compiler
This is my submission for the compiler project.

# Extensions
In addition to all required functionality, I have added the following extension to my project:
1. Allowing float data types. This language allows one to declare a float or an int variable type. The language uses C style implicit conversions. In an arithmetic operation between an int and a float, the int is converted to a float before the operation. If an int is assigned to a float (or vice versa), there is an implicit type conversion. When accepting user input, a number is converted to an int or float automatically based on the variable to which it is assigned.
2. Implemneting arrays. A user can declare an array with the syntaxt `int x[5];`, initalize an array with the syntax `x = {1,3,5};` OR `x[3] = 7;`, and access the array with the syntax `x[2]`. Arrays have constant length.
3. Implementing basic vectorized array operations. Once an array has been created, it can be modified with syntax like `x = x * (y + 2) * 4`. In this example, each value in x is multiplied by `(y+2) * 4`. 

# To use compiler
- The command **make** will compile the compiler and create an executable called c.exe
- The command **make stack** will compile the stack machine and create an executable called s.exe
- To compile using my compiler, use the command: **./c.exe filename.txt**, where **filename.txt** is your program. This will produce a file called **filename.txt.vsm**, which is the compiled output containing valid stack machine code. 
- To test the stack machine code, use the command **./s.exe filename.txt.vsm**. The output will be printed to standard out.

# Files in this directory
- **documentation/LT_Compiler_Grammar.docx**: a document defining the language used in generating my Abstract Syntax Tree. Note that the Code Generator uses a slightly different set of rules. For instance, the Code Generator combines Rules 25 and 26 into one.
- **examples/**: a directory containing a handful of files used as inputs or outputs for tests. `gcd_example.txt` is the GCD code we went over in class. `float_test.txt` is a simple program to test that the float data type was implemented correctly. The `array_test.txt` files test different parts of my array implementations.
- **ast.h** and **ast.cpp**: Defines the ASTNode, SymbolTable, and Parser classes used in my compiler.
- **codeGenerator.h** and **codeGenerator.cpp**: Defines the CodeGenerator for my compiler.
- **token.h** and **token.cpp**: Defines the Token class used in my compiler.
- **stackMachine.cpp**: Defines the stack machine that serves as the target language.
- **lexer.cpp**: Conntains code to perform lexing for my compiler. Also contains the main() function called by my compiler.
- **stackMachineMain.cpp**: Contains the main function for my stack machine

# Known Limitations
1. I do not currently check that the right number of parameters are passed to a function at compile time. I also don't check parameter type. If too few or many parameters are passed into a function, the stack machine will generate unpredictable output. Similarly, if a list is passed as a parameter into the function, the first value in the list will be used as the parameter. 
2. There is no run-time error handling. If user input is used to access an element from an array, for example, there is no guarantee they will not try to access a value out of bounds.
3. In any given function, each variable must be declared before any other statements are made. (This is by design of the language and not really a limitation).

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
- INT(), FLOAT()

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