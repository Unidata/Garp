#include <stdio.h>
#include <string.h>

#ifdef ULTRIX
char *strdup(string1)
char *string1;
{
char *string2;

string2 = (char *) malloc((strlen(string1)+1) * sizeof(char));
strcpy(string2,string1);
return(string2);
}
#endif
