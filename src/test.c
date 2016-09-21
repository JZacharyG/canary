#include "canary.h"
#include "stopwatch.h"
#include <stdio.h>
#include <string.h>

void minorTest(const char* Gstr, const char* Hstr, int hasMinor)
{
	setgraph G,H;
	g62setgraph(Gstr, &G);
	g62setgraph(Hstr, &H);
	
	printf("Testing '%s' %s '%s'\n", Hstr, (hasMinor)?"<":"!<", Gstr);
	stopwatch s;
	reset_stopwatch(s);
	start_stopwatch(s);
	int result = has_minor(&G, &H, NULL);
	pause_stopwatch(s);
	if (!hasMinor != !result)
		fprintf(stderr, "TEST FAILED\n\tdesired: %s\n\tresult: %s\n\n", (hasMinor)?"true":"false", (result)?"true":"false");
	else
	{
		printf("\tTest Passed (");
		print_stopwatch(s);
		printf(")\n\n");
	}
	free_setgraph(&G);
	free_setgraph(&H);
}

void minorTestFile(const char* filename, const char* Hstr, int hasMinor)
{
	int numFailed = 0, numPassed = 0;
	FILE* file = fopen(filename, "r");
	if (file == NULL)
	{
		fprintf(stderr, "File not found: '%s'\nPlease run this program while in the main directory for canary.\n\n", filename);
		return;
	}
	printf("Testing file: '%s' for '%s'\n", filename, Hstr);
	size_t Gstrlen = 20;
	char* Gstr = malloc(Gstrlen);
	setgraph G,H;
	g62setgraph(Hstr, &H);
	stopwatch s,t;
	reset_stopwatch(s);
	while ((getline(&Gstr, &Gstrlen, file)) > 1)
	{
		g62setgraph(Gstr, &G);
		start_stopwatch(s);
		int result = has_minor(&G, &H, NULL);
		pause_stopwatch(s);
		if (!hasMinor != !result)
		{
			// FIX ME: assumes that there is a new line at the end of the string
			// obtained from getline.  Should probably handle this better.
			fprintf(stderr, "TEST FAILED: %s < %s\tdesired: %s\n\tresult: %s\n", Hstr, Gstr, (hasMinor)?"true":"false", (result)?"true":"false");
			++numFailed;
		}
		else
			++numPassed;
		free_setgraph(&G);
	}
	free_setgraph(&H);
	fclose(file);
	free(Gstr);
	printf("Done testing '%s'\n\t%d tests passed and %d tests failed.\n\tTotal Time: ", filename, numPassed, numFailed);
	print_stopwatch(s);
	printf("\n\n");
}