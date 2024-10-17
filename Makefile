all: shell
run: clean shell
	@clear
	@./hy345sh
shell: hy345sh.c
	@gcc  hy345sh.c -o hy345sh -g
	@echo "Shell compiled"
	@echo "Usage: ./hy345sh"
clean:
	@rm -rf hy345sh
	@echo "Cleaned!"
#-pedantic -Wall -g