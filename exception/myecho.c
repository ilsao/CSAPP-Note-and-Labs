/* myecho.c */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[])
{
	printf("Command-line arguments:\n");
	int i;
	for (i = 0; i < argc; i++)
		printf("\targv[%2d]: %s\n", i, argv[i]);
	printf("Environment variables:\n");
	i = 0;
	while (envp[i] != NULL)
		printf("\tenvp[%2d]: %s\n", i, envp[i++]);
	return 0;
}