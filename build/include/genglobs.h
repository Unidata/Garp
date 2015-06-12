/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	genglobs.h
 *
 *	General global variables, default definitions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 5/97	COMET	Added macros for logging.
 *	 5/97	COMET	Added saveImageData to GlobalDataObjectType.
 *	 6/97	COMET	Added implicitImageReset to GlobalDataObjectType.
 *	12/97	COMET	Added text characteristics to GlobalDataObjectType.
 *	12/97	COMET	Added a VERBOSE_4 macro.
 *	 2/98	COMET	Added STIDSIZE.
 *	 3/98	COMET	Added showTitle to GlobalDataObjectType.
 *
 ***********************************************************************/

#ifndef __GEMGLOBS_H
#define __GEMGLOBS_H


#include "gendef.h"
#include "gui.h"

#define HELP1_OPTION           "-help"
#define HELP2_OPTION           "-h"
#define CONFIGFILE_OPTION      "-config"
#define NOTICK_OPTION          "-notick"
#define STARTTIME1_OPTION      "-start"
#define STARTTIME2_OPTION      "-start_time"
#define ENDTIME1_OPTION        "-end"
#define ENDTIME2_OPTION        "-end_time"
#define CURRENTTIME1_OPTION    "-current"
#define CURRENTTIME2_OPTION    "-current_time"
#define TABLEDIR_OPTION        "-tabledir"
#define VERBOSE_OPTION         "-verbose"
#define OUTPUT_OPTION          "-output"
#define NOICONS_OPTION         "-noicons"
#define NOSTATUS_OPTION        "-nostatus"
#define SWIDTH_OPTION          "-width"
#define SHEIGHT_OPTION         "-height"
#define CLOCKINTERVAL_OPTION   "-clockinterval"
#define MEMCHECK_OPTION        "-memcheck"
/*
 *
 *	Symbolic names for configuration file keys
 *
 */
#define PROGNAME	"Progname"
#define CONFIGFILE	"ConfigFile"
#define CLOCKINTERVAL	"ClockInterval"
#define NOTICK	        "NoTick"
#define NOICONS	        "NoIcons"
#define SHEIGHT	        "Height"
#define SWIDTH	        "Width"
#define NOSTATUS   	"NoStatus"
#define STARTTIME	"StartTime"
#define ENDTIME		"EndTime"
#define CURRENTTIME	"CurrentTime"
#define METDAT		"Metdat"
#define VERBOSE		"Verbose"
#define OUTPUT		"Output"
#define SATDIR		"SatDir"
#define NIDSDIR		"NidsDir"
#define SURFACEDIR	"SurfaceDir"
#define GRIDSDIR	"GridsDir"
#define UPPERAIRDIR	"UpperAirDir"
#define PROFILERDIR	"ProfilerDir"
#define MEMCHECK        "MemCheck"

#define MASTERTBL	"MasterTable"
#define SATENHANCETBL	"SatEnhanceTable"
#define RADENHANCETBL	"RadEnhanceTable"
#define TABLEDIR	"TableDir"
#define TEMPLATEFILE	"TemplateFile"

#define LOWRESCOSTAL			"LowResCostal"
#define MEDRESPOLITICALWORLD		"MediumResPoliticalWorld"
#define HIRESPOLITICALWORLD		"HiResPoliticalWorld"
#define HIRESCOUNTYUS			"HiResCountyUS"

#define MINZOOMBOXSIZE  "minzoomboxsize"
#define ZOOMBOXCOLOR	"zoomboxcolor"

#define LATLON		"latlon"
#define LATINC	  	"latinc"
#define LONINC		"loninc"
#define LBLINC		"lblinc"
#define GRIDLINECOLOR	"gridlinecolor"
#define GRIDLINETYPE	"gridlinetype"
#define GRIDLINEWIDTH	"gridlinewidth"

/*
 *
 *	Strings to drive configfile parameter conversion.
 *
 */
#define STRING_CONVERT		0
#define INT_CONVERT		1
#define FLOAT_CONVERT		2


/* 
 *	Verbose levels
 */
#define VERBOSE_0       0       /* Silence  */
#define VERBOSE_1       1       /* Function names */
#define VERBOSE_2       2       /* VERBOSE_1 + Function contents */
#define VERBOSE_3       3	/* VERBOSE_2 + Station table contents */
#define VERBOSE_4       4	/* VERBOSE_3 + Lots of gridded data info */


/*
 *	Print levels
 */
#ifndef _NO_GPRINT
#define printf			gprint	/* Garp print function */
#endif
#define PRINT_TO_STDOUT		0	/* Print to standard out */
#define PRINT_TO_LOG_WINDOW	1	/* Print to GARP log window */
#define PRINT_TO_FILE		2	/* Print to a file */

/*
 *	Logging options
 */
#define STANDARD_OUT	0	/* Log to standard error */
#define LOG_WINDOW	1	/* Log to GARP logging window */

/*
 *	Status Bar items (don't use 0, these values are negated when
 *	not in use)
 */
#define	CLOCK		1
#define WINDOWCOORDS	2
#define	PLOTCOORDS	3
#define PIXELVALUE	4
#define	DISTCALC	5



#define MAX_COLORS 10	/* Max number of colors in color list */
#define MAX_WINPOS  3   /* Max number of wind positions in upper air profiles */


#define STIDSIZE 9	/* Max size of station id strings */

/* 
 *  Global variables structure 
 */

typedef struct {
	int		debug;
	int		memCheck;
	int	        verbose;
	BooleanType	noIcons;
	BooleanType	noStatus;
	BooleanType	showTitle;
	int		output;
	int		startingWidth;
	int		startingHeight;

	char		*startTime;
	char		*endTime;
	char		*currentTime;
	BooleanType	noTick;

	char		appfile[81];
	char		color_table[200]; /* belongs in satobj */
	BooleanType	retain_ct;        /* belongs in satobj */
	BooleanType	saveImageData;
	BooleanType	implicitImageReset; /* reset loading images */
	char		bg_colors[81];
	char		fg_colors[81];
	char		color_list[MAX_COLORS][4];
	BooleanType	color_used[MAX_COLORS];
	int		color_count;
	char		colors[81];
	char		text_size[81];
	char		text_font[81];
	char		text_width[81];
	char		text_hw_flag[81];
	BooleanType	winpos_used[MAX_WINPOS]; /* UPA wind position */
	char		winpos[81];
	char		search_string[81]; /* for file filtering */
} GlobalDataObjectType;


GlobalDataObjectType *GetGlobalDataObject();
char	*GetConfigValue( char *key );


/*
 *	Macros to access globalData structure.
 */

#define SetConfigFile(c)      (strcpy((GetGlobalDataObject())->appfile,(c)))
#define GetConfigFile()       ((GetGlobalDataObject())->appfile)

#define SetVerboseLevel(c)    ((GetGlobalDataObject())->verbose = (c))
#define GetVerboseLevel()     ((GetGlobalDataObject())->verbose)

#define SetOutputLevel(c)     ((GetGlobalDataObject())->output = (c))
#define GetOutputLevel()      ((GetGlobalDataObject())->output)

#define SetClockStopped(c)    ((GetGlobalDataObject())->clockStopped = (c))
#define GetClockStopped()     ((GetGlobalDataObject())->clockStopped)

#define SetCurrentTime(c)     ((GetGlobalDataObject())->currentTime = (c))
#define GetCurrentTime()      ((GetGlobalDataObject())->currentTime)

#define SetEndTime(c)         ((GetGlobalDataObject())->endTime = (c))
#define GetEndTime()          ((GetGlobalDataObject())->endTime)

#define SetStartTime(c)       ((GetGlobalDataObject())->startTime = (c))
#define GetStartTime()        ((GetGlobalDataObject())->startTime)

#define SetNoTick(c)          ((GetGlobalDataObject())->noTick = (c))
#define GetNoTick()           ((GetGlobalDataObject())->noTick)

#define SetNoIcons(c)          ((GetGlobalDataObject())->noIcons = (c))
#define GetNoIcons()           ((GetGlobalDataObject())->noIcons)

#define SetNoStatus(c)         ((GetGlobalDataObject())->noStatus = (c))
#define GetNoStatus()          ((GetGlobalDataObject())->noStatus)

#define SetStartingWidth(c)    ((GetGlobalDataObject())->startingWidth = (c))
#define GetStartingWidth()     ((GetGlobalDataObject())->startingWidth)

#define SetStartingHeight(c)   ((GetGlobalDataObject())->startingHeight = (c))
#define GetStartingHeight()    ((GetGlobalDataObject())->startingHeight)

#define SetDefColortable(g,c)		(strcpy( (g)->color_table, (c) ))
#define GetDefColortable(g)		((g)->color_table)

#define SetDefColortableRetain(g,c)	((g)->retain_ct = (c))
#define GetDefColortableRetain(g)	((g)->retain_ct)

#define SetSaveImageDataFlag(g,c)	((g)->saveImageData = (c))
#define GetSaveImageDataFlag(g)		((g)->saveImageData)

#define SetImageResetFlag(g,c)		((g)->implicitImageReset = (c))
#define GetImageResetFlag(g)		((g)->implicitImageReset)

#define SetTitleFlag(g,c)		((g)->showTitle = (c))
#define GetTitleFlag(g)			((g)->showTitle)

#define SetBackgroundColors(g,c)	(strcpy( (g)->bg_colors, (c) ))
#define GetBackgroundColors(g)          ((g)->bg_colors)

#define SetForegroundColors(g,c)	(strcpy( (g)->fg_colors, (c) ))
#define GetForegroundColors(g)		((g)->fg_colors)

#define SetObjectColors(g,c)		(strcpy( (g)->colors, (c) ))

#define SetSearchString(g,c)		(strcpy( (g)->search_string, (c) ))
#define GetSearchString(g)		((g)->search_string)

#endif  /*  __GENGLOBS_H  */

