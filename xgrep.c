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
	
static char * proc_file(FILE *file)	{
// Read the file line by line,
// Check for switches
// To determine the output

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
	}

int main(int argc, char **argv)	{
	// Main function
	regex_t c_regexp;
	int return_code;
	FILE *input_file;
	char *proc_switch_error;
	char *prog_name = argv[0];
	char *error_msg;

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
		
	return_code = xregcomp(&c_regexp, argv[0], REG_NEWLINE | REG_ENHANCED);
	if (return_code != 0)	{
		fprintf(stderr, "Error: %s, pat '%s'.\n", xregmsg(return_code), argv[1]);
		exit(1);
		}
	
	argc--;
	argv++;
	
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
					
		if ((error_msg = proc_file(input_file)) != NULL)	{
			fprintf(stderr,"File error: %s\n", error_msg);
			exit(1);
			}

		(void) fclose(input_file);
		if (argc <= 0)
			break;
		}
	return 0;
}
