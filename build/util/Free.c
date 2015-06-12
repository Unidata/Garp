#include <stdlib.h>
#include <stdio.h>
#include "genglobs.h"
#include "_proto.h"


void Free(void *x)
{
    if (((x) == (void *)NULL) || ((x) == (void *) 0xffffffff)) {
	printf("!!!!!Whoa!!!!!!  Free() got a 0x%x\n", (x));
        if( ! GetMemCheck() ) 
	    free((x));
    } 
    else {
        free(x);
        x = (void *) 0xffffffff;
    }
}
