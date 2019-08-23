CC   = gcc
NAME = pdfmanip

W_FLAGS = -Wall -Werror -Wshadow -Wformat-security -Wimplicit-function-declaration -Wpointer-arith -Wcast-align -Wredundant-decls  -pedantic

C_FLAGS = -g3 $(W_FLAGS) $(INCLUDES)
L_FLAGS = -lz -lm
D_FLAGS = -ggdb $(W_FLAGS)

O_FILES = pdfmanip.o main.o list.o utils.o

all: $(O_FILES)
	rm -f $(NAME)
	$(CC) -o $(NAME) $(O_FILES) $(L_FLAGS) $(C_FLAGS) $(D_FLAGS)

.c.o: all
	@$(CC) -c $(C_FLAGS) $<

clean:
	@echo Cleaning code $< ...
	@rm -rf *.o
	@rm -f $(NAME)
	@rm -f *.*~
