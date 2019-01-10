
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/*
 * The elf_mangle program generates a translation file, that contain previous and new values for all defined symbols
 * 	in all provided elf files; This file can be reused for renaming with objcopy;
 *
 * The following set of arguments must be provided :
 * 	- p : the prefix to add to all defined symbols in object files; mandatory
 * 	- o : the name of the output file;
 *
 *
 *
 */



int main(int argc, char **argv) {
	
	char *pvalue = 0;
	
	
	char *ovalue = 0;
	
	char *ivalue = 0;
	
	opterr = 0;
	
	int c;
	
	while ((c = getopt(argc, argv, "hfp:o:i:")) != -1) {
		
		switch (c) {
			case 'h':
				printf("elf_mangle : generates a translation table for the provided file.\n\r"
						   "\t-h prints this message;\n\r"
						   "\t-p the prefix to add to all symbol names\n\r"
						   "\t-f the file to generate the translation table for\n\r");
				exit(1);
			
			case 'p':
				pvalue = optarg;
				break;
			case 'o':
				ovalue = optarg;
				break;
			case 'i':
				ivalue = optarg;
				break;
			
			default:
				abort();
			
		}
		
	}
	
	
	const size_t name_size = strlen(ivalue);
	const size_t command_size = 27 + name_size;
	
	const size_t prefix_size = strlen(pvalue);
	
	//Allocate some memory for the command;
	char *command = malloc(command_size);
	
	//Copy the command;
	memcpy(command, "nm -p ", 6);
	memcpy(command + 6, ivalue, name_size);
	memcpy(command + 6 + name_size, " > /tmp/_elf_mgl.tmp\0", 21);
	
	system(command);
	
	free(command);
	
	FILE *src = fopen("/tmp/_elf_mgl.tmp", "r");
	FILE *dst = fopen(ovalue, "w");
	
	char *line;
	size_t len;
	
	while (getline(&line, &len, src) != -1) {
		
		char *id = line;
		
		while ((*id != ' ') && (*id != '\0')) {
			id++;
		}
		
		while (*id == ' ') {
			id++;
		}
		
		
		if ((*id == 'T') || (*id == 'D') || (*id == 'R')) {
			
			id++;
			
			while (*id == ' ') {
				id++;
			}
			
			if ((*id == '$')) {
				continue;
			}
			
			const char *symbol = id;
			
			while ((*id != ' ') && (*id > '\n')) {
				id++;
			}
			
			*id = 0;
			
			
			fprintf(dst, "%s %s_%s\n", symbol, pvalue, symbol);
		}
		
		
	}
	
	fclose(src);
	fclose(dst);
	
	system("rm /tmp/_elf_mgl.tmp");
	
	return 0;
	
}
