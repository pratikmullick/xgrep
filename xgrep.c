#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "xre.h"
#include <errno.h>

int main(int argc, char **argv)	{
	// Main function
	regex_t c_regexp;
	int return_code;
	FILE *input_file;
	
	if (argc < 2)	{
		// Something's wrong
		fprintf(stderr, "Usage: %s regex [file ...]\n", argv[0]);
		exit(1);
		}
	
	return_code = xregcomp(&c_regexp, argv[1], REG_NEWLINE | REG_ENHANCED);

	if (return_code != 0)	{
		fprintf(stderr, "Error: %s, pat '%s'.\n", xregmsg(return_code), argv[1]);
		exit(1);
		}
	
	argc -= 2;
	argv += 2;
	for (;;)	{
		if (argc == 0)	
			input_file = stdin;
		else		{
			input_file = fopen(*argv,"r");
			if (input_file == NULL)	{
				fprintf(stderr,"%s File Open Error %s\n", *argv, strerror(errno));
				exit(1);
			}
		}
		argc--; argv++;
					
		for (;;)	{
			char *line = NULL;
			size_t len = 0;
			regmatch_t match[10];
			if (getline(&line, &len, input_file) < 0)	{
				if (feof(input_file))
					break;
				fprintf(stderr, "Read Error: %s\n", strerror(errno));
				exit(1);
				}
			return_code = xregexec(&c_regexp, line, 10, match, 0);
			if (return_code == 0)	{
				// Got a match.
				fputs(line, stdout);
				}
			else if (return_code != REG_NOMATCH)	{
				fprintf(stderr, "Regular Expression Error: %s\n", xregmsg(return_code));
				exit(1);
				}
			free(line);
			}
		(void) fclose(input_file);
		if (argc <= 0)
			break;
		}
	return 0;
}
