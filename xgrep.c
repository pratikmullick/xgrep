/* A simple Quicksort Program. Copyright 2019 2020. Pratik Mullick and 
 * Richard W. Marinelli. Please read LICENSE.md for further details. */

/* 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
