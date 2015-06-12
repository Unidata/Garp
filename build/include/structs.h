/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	structs.h
 *
 *	Some structure declarations
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#define MAXTEMPLATEOBS 15
#define DEF_LENGTH 81
#define TEMPLATE_SIZE 10
#define INPUT_SIZE 1000



/* Template structure */

struct template_struct {
        char    name[DEF_LENGTH];
        char    value[DEF_LENGTH];
} ;


/* Input structure */

struct input_struct {
        char    name[DEF_LENGTH];
        char    value[DEF_LENGTH];
} ;



/* Name/value structure */

typedef struct {
	char	*name;
	char	*value;
} NameValueObjectType;

typedef struct {
	char			*plot_type;
	int			entry;
/*	NameValueObjectType	*namevalueObj[MAXTEMPLATEOBS]; */
} TemplateObjectType;
