#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "xre.h"
#include <errno.h>

static bool h_switch, H_switch, i_switch, v_switch, l_switch, n_switch, o_switch, c_switch;

static char * proc_switches(int * p_argc, char ***p_argv)	{
	// Local variables for argc and argv
	int argc = *p_argc;
	char **argv = *p_argv;
	char *sw;
	char *error_msg;
	
	// If not an error returns NULL, else returns error message.
	while (argc > 0 && **argv == '-')	{
		sw = *argv++;
		--argc;
		if (sw[2] != '\0')
			goto BadSwitch;
		switch (sw[1])	{
			default:
BadSwitch:
				asprintf(&error_msg, "Invalid switch: '%s'", sw);
				return error_msg;
			case 'h':
				h_switch = true;
				break;
			case 'H':
				H_switch = true;
				break;
			case 'i':
				i_switch = true;
				break;
			case 'v':
				v_switch = true;
				break;
			case 'l':
				l_switch = true;
				break;
			case 'n':
				n_switch = true;
				break;
			case 'o':
				o_switch = true;
				break;
			case 'c':
				c_switch = true;
				break;
			}
		}
	*p_argc = argc;
	*p_argv = argv;

	return NULL;
	}

static char *proc_file(FILE *input_file, regex_t *pc_regexp, long *match_count, char *filename)	{
// Read the file line by line,
// Check for switches
// To determine the output
	char *msg = NULL;
	int return_code;
	long line_number = 0;
	static char err_message[200];
	for (;;)	{
		char *line = NULL;
		size_t len = 0;
		regmatch_t match[10];
		if (getline(&line, &len, input_file) < 0)	{
			if (feof(input_file))
				break;
			sprintf(msg = err_message, "Read Error: %s\n", strerror(errno));
			break;
			}
		// Counting line numbers
		line_number++;
		return_code = xregexec(pc_regexp, line, 10, match, 0);
		if ((return_code == REG_NOMATCH && v_switch == true) || (return_code == 0 && v_switch == false))	{

			// Got a match.
			if (c_switch)
				(*match_count)++;
			else	{
				if (filename != NULL)
					printf("%s:", filename);
				if (n_switch)
					printf("%ld:", line_number);
				if (o_switch)
					// Prints only the part of the line that matched
					printf("%.*s\n", (int) (match[0].rm_eo - match[0].rm_so), line + match[0].rm_so);
				else
					fputs(line, stdout);
				}
			}
		else if (return_code != REG_NOMATCH && return_code != 0)	{
			sprintf(msg = err_message, "Regular Expression Error: %s\n", xregmsg(return_code));
			break;
			}
		free(line);
		}
	return msg;
	}

int main(int argc, char **argv)	{
	// Main function
	regex_t c_regexp;
	int return_code;
	FILE *input_file;
	char *proc_switch_error;
	char *prog_name = argv[0];
	char *error_msg;
	long match_count;
	bool filename_print = false;

	--argc;
	++argv;
	
	if ((proc_switch_error = proc_switches(&argc,&argv)) != NULL)	{
		fprintf(stderr,"Switch error: %s\n", proc_switch_error);
		exit(1);
		}
	if (argc < 1)	{
		// Something's wrong
		fprintf(stderr, "Usage: %s [-c] [-h] [-H] [-i] [-l] [-n] [-o] [-v] regex [file ...]\n", prog_name);
		exit(1);
		}
#if 1		
	return_code = xregcomp(&c_regexp, argv[0], i_switch ? REG_ICASE | REG_NEWLINE | REG_ENHANCED : REG_NEWLINE | REG_ENHANCED);
	if (return_code != 0)	{
		fprintf(stderr, "Error: %s, pat '%s'.\n", xregmsg(return_code), argv[1]);
		exit(1);
		}
	
	argc--;
	argv++;

	if (argc > 1 && !h_switch)	{
		// switch to enable file name printing
		filename_print = true;
	}
	// For loop to check for match in each file. If no file is specified, read from stdin.
	for (;;)	{
		match_count = 0;
		if (argc == 0)	
			input_file = stdin;
		else		{
			input_file = fopen(*argv,"r");
			if (input_file == NULL)	{
				fprintf(stderr,"%s File Open Error %s\n", *argv, strerror(errno));
				exit(1);
			}
			argc--; argv++;
		}
					
		if ((error_msg = proc_file(input_file, &c_regexp, &match_count, filename_print ? *(argv - 1) : NULL)) != NULL)	{
			fprintf(stderr,"File error: %s\n", error_msg);
			exit(1);
			}

		(void) fclose(input_file);
		if (c_switch)
			printf("%ld\n", match_count);

		// Just going through the loop once, prevents reading from stdin if a file was specified.
		if (argc == 0)
			break;
		}
#else
	// Print out all switch, argc, and argv values.
	fprintf(stderr, "Switch values: h %d, H %d, i %d, v %d, l %d, n %d, o %d, c %d\n", h_switch, H_switch, i_switch, v_switch, l_switch, n_switch, o_switch, c_switch);
	fprintf(stderr, "Argc: %d\n", argc);
	while (argc > 0)	{
		fprintf(stderr, "Argv: %s\n", *argv++);
		argc--;
		}
#endif
	return 0;
}
