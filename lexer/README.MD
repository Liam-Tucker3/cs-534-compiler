# Lexer
This project is part one of a semester-long project building a compiler. My goal here is to build a lexer. The re is no 

# AI Statement
For this assignment, I have utilized the **Github Copilot** extention in VSCode, a tool created by **Github**. This uses the **GPT-4o** model created by **OpenAI**. I have used this tool for the following purposes:
- Automating repetitive tasks (i.e. filling all 20+ lines of a toString() function). This avoids me needing to write the same simple logic 20 times
- Splitting a token.cpp class into two files, token.h and token.cpp
- Writing extremely basic templates for certain functions
- Autocompleting certain lines of code, particularly in simple functions like Token::toString
- Generating the list of C++ keywords

# TODO
1. Modify lexer.cpp to accept a filename as a parameter and parse the file, rather than one input string
2. Add extra punctuation to lexer as needed
3. Modify lexer.cpp to generate vector of Tokens instead of simply printing tokens to string