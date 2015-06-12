/***********************************************************************
 *
 *	Copyright 1997, University Corporation for Atmospheric Research.
 *
 *	template.h
 *
 *	Time utility declarations
 *
 *	History:
 *
 *	 8/97	COMET	Original copy
 *
 ***********************************************************************/


#ifndef __TEMPLATE_H
#define __TEMPLATE_H



typedef struct {
	char		**list;
	int		size;
} TemplateObjectType;


/* Macros */



/* Prototypes */

TemplateObjectType *GetTemplateObject();


#endif   /*  __TEMPLATE_H  */
