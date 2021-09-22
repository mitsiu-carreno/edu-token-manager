# Determine the platform
PLAT_NAME := $(shell uname -s)

# Compiler
ifeq ($(PLAT_NAME), Darwin)
  CC=clang++ -arch x86_64
  LIBS= -lncurses
else
  CC=g++
  LIBS= -lncurses -lmysqlclient
endif

# Project structure
SRC_DIR := src
BUILD_TARGET := build
EXEC_TARGET := bin

# Executable
EXEC_TARGET_NAME := token_manager
EXEC_FULL_PATH := $(EXEC_TARGET)/$(EXEC_TARGET_NAME)

# Code properties
SRC_EXT := cpp
# Find all the .cpp
SRC_FULL_PATH_LIST := $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
# Based on SRC_FULL_PATH_LIST create the list of *.o files
O_FILE_FULL_PATH_LIST := $(patsubst $(SRC_DIR)/%,$(BUILD_TARGET)/%,$(SRC_FULL_PATH_LIST:.$(SRC_EXT)=.o))
# Based on O_FILE_FULL_PATH_LIST remove the *.o file (just leave the directory structure)
BUILD_DIRECTORY_LIST:=$(foreach O_FILE,$(O_FILE_FULL_PATH_LIST),$(subst $(notdir $(O_FILE)),,$(O_FILE)))
# Remove the duplicates
BUILD_DIRECTORY_LIST := $(sort $(BUILD_DIRECTORY_LIST))

# Find all the directories containing .hpp and .h files
HEADER_DIRECTORY_LIST := $(shell find include \( -name '*.hpp' -o -name '*.h' \) -exec dirname {} \; | sort | uniq)
# Prefix -I to all directories containing .hpp and .h files
INCLUDE_LIST := $(patsubst include%,-I include%,$(HEADER_DIRECTORY_LIST))

#ggdb3 helps analyse with valgrind
CFLAGS=-c -std=c++1z -Wall -Werror -g -ggdb3 -I/usr/include/mysql 

$(EXEC_TARGET): $(O_FILE_FULL_PATH_LIST)
	@mkdir -p $(EXEC_TARGET)
	@echo "Linking..."
	@echo "		$(CC) $^ $(LIBS) -o $(EXEC_FULL_PATH)"; $(CC) $^ $(LIBS) -o $(EXEC_FULL_PATH);
	@echo "Build complete, executing..."
	@echo ""
	./$(EXEC_FULL_PATH) "127.0.0.1" "my_user" "my_password" "my_database"

$(BUILD_TARGET)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@mkdir -p $(BUILD_DIRECTORY_LIST)
	@echo "Compiling $<..."
	@echo "$(CC) $(CFLAGS) $(INCLUDE_LIST) $< -o $@"; $(CC) $(CFLAGS) $(INCLUDE_LIST) $< -o $@

run:
	./$(EXEC_FULL_PATH) "127.0.0.1" "my_user" "my_password" "my_database"

debug:
	@echo "SRC_FULL_PATH_LIST $(SRC_FULL_PATH_LIST)"
	@echo "O_FILE_FULL_PATH_LIST $(O_FILE_FULL_PATH_LIST)"
	@echo "BUILD_DIRECTORY_LIST $(BUILD_DIRECTORY_LIST)"
	@echo ""
	@echo "HEADER_DIRECTORY_LIST $(HEADER_DIRECTORY_LIST)"
	@echo "INCLUDE_LIST $(INCLUDE_LIST)"

clean:
	@echo "Cleaning $(EXEC_FULL_PATH) ..."; $(RM) -r $(EXEC_FULL_PATH)

hardclean:
	@echo "Cleaning $(EXEC_FULL_PATH) and $(BUILD_TARGET)/* ..."; $(RM) -r $(BUILD_TARGET)/* $(EXEC_FULL_PATH)

.PHONY: clean	
