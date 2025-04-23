# Makefile for compiling the project

# Makefile Documentation
#
# This Makefile is designed to automate the build, test, and deployment process for the project.
# Below are the instructions for using this Makefile:
#
# Targets:
# - `all`: The default target that compiles the entire project.
# - `clean`: Removes all generated files and resets the build environment.
# - `build`: Compiles the source code into executable binaries.
# - `test`: Runs the test suite to verify the correctness of the code.
# - `install`: Installs the compiled binaries and dependencies to the system.
# - `uninstall`: Removes the installed binaries and dependencies from the system.
# - `stack`: Compiles the stack machine executable.
#
# Usage:
# 1. To build the project, run: `make` or `make all`.
# 2. To clean the project directory, run: `make clean`.
# 3. To compile the source code, run: `make build`.
# 4. To execute tests, run: `make test`.
# 5. To install the project, run: `make install`.
# 6. To uninstall the project, run: `make uninstall`.
# 7. To build the stack machine, run: `make stack`.
#
# Notes:
# - Ensure that all dependencies are installed before running the Makefile.
# - Modify the variables section of the Makefile to customize paths or compiler options if needed.
# - Run `make help` (if implemented) to see a list of all available targets and their descriptions.



# Compiler
CXX = g++



# Source files
SRC = token.cpp ast.cpp codeGenerator.cpp lexer.cpp
STACK_SRC = stackMachine.cpp stackMachineMain.cpp

# Output executable
OUT = c
STACK_OUT = s

# Build target
all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(SRC) -o $(OUT)

# Stack machine target
stack: $(STACK_SRC)
	$(CXX) $(STACK_SRC) -o $(STACK_OUT)

# Clean target
clean:
	rm -f $(OUT) $(STACK_OUT)