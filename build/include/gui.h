/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gui.h
 *
 *	Main GUI header file.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <sys/utsname.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <X11/X.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/BulletinB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/FileSB.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/DrawingA.h>
#include <Xm/Separator.h>
#include <Xm/SeparatoG.h>
#include <Xm/MessageB.h>
#include <Xm/Scale.h>
#include <Xm/CascadeB.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include <Xm/CascadeBG.h>
#include <Xm/PushBG.h>
#include <Xm/PanedW.h>
#include <Xm/SelectioB.h>
#include <Xm/ArrowBG.h>
