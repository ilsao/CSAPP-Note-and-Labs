#ifdef RUNTIME
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

/* malloc wrapper funciton */
void *malloc(size_t size)
{
	void *(*mallocp) (size_t size);
	char *error;

	mallocp = dlsym(RTLD_NEXT, "malloc");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}
	char *ptr = mallocp(size); /* Call libc malloc */
	// printf("malloc(%d) = %p\n", (int)size, ptr);
	return ptr;
}

void free(void *ptr)
{
	void (*freep) (void *) = NULL;
	char *error;

	if (!ptr)
		return;

	freep = dlsym(RTLD_NEXT, "free");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}
	freep(ptr);
	// printf("free(%p)\n", ptr);
}
#endif