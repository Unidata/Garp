/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	utils.h
 *
 *	List structures.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#define MAX_LIST 10000

struct list_struct {
	char	**file;
	char	**list;
	int	list_size;
	int	selected_item_count;
};


struct data_struct {
        int     list_size;
};

struct enhance_list {
	char	**list;
	char	**background;
	char	**foreground;
	int	size;
};

/*#endif */ /*  __UTILS_H  */
