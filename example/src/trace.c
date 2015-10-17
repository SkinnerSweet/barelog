/* The MIT License (MIT)

 Copyright (c) 2015 Thomas Bertauld <thomas.bertauld@gmail.com>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include "trace.h"
#include <stdio.h>
#include <string.h>

#define style(c) fprintf(stderr, "\033[%s",c);

#define DEFAULT "0;m"

#define BLACK "0;30m"
#define RED "0;31m"
#define GREEN "0;32m"
#define YELLOW "0;33m"
#define BLUE "0;34m"
#define WHITE "0;37m"

#define BOLD "1m"

#define HIGHLIGHT "7m"
#define ERROR "7;30;1;31;1m"

static trace_lvl_t trace_lvl = TRACE_DEBUG;

void set_trace_lvl(trace_lvl_t lvl) {
	trace_lvl = lvl;
}

void print_trace(trace_lvl_t lvl, char *string) {
	if (lvl <= trace_lvl) {
		switch (lvl) {
		case TRACE_ERROR:
			style(ERROR);
			fprintf(stderr, "[ERROR] %s", string);
			style(DEFAULT);
			break;
		case TRACE_STDOUT:
			fprintf(stderr, "%s", string);
			break;
		case TRACE_WARNING:
			style(YELLOW);
			style(BOLD);
			fprintf(stderr, "[WARNING] ");
			style(DEFAULT);
			style(YELLOW);
			fprintf(stderr, "%s", string);
			style(DEFAULT);
			break;
		case TRACE_INFO:
			style(GREEN);
			style(BOLD);
			fprintf(stderr, "[INFO] ");
			style(DEFAULT);	
			style(GREEN);
			fprintf(stderr, "%s", string);
			style(DEFAULT);
			break;
		case TRACE_DEBUG:
			style(WHITE);
			style(BOLD);
			fprintf(stderr, "[DEBUG] ");
			style(DEFAULT);
			style(WHITE);
			fprintf(stderr, "%s", string);
			style(DEFAULT);
			break;
		default:
			fprintf(stderr, "[TRACE_ERROR] : unrecognized trace lvl.\n");
			break;
		}
	}
}
