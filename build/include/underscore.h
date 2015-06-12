/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	underscore.h
 *
 *	Defines underscore versions of functions. Solaris issue.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 5/97	COMET	Added macro for gfprint.
 *	 6/97	COMET	Added several gempak and garp routines
 *	10/97	COMET	Added countsfcobs and countupaobs, pllgrid,
 *			pbgmap. Removed mapdraw.
 *	11/97	COMET	Added dscolr, ptitle.
 *	 2/98	COMET	Added DOUBLE_UNDERSCORE list of items 
 *	 3/98	COMET	Added parse_gfunc
 *
 ***********************************************************************/


#ifndef __UNDERSCORE_H

#define __GEMPAKREF_H

/*
 *	This is a tricky one.  In a few cases we include
 *	the xwcmn.h file, but not very often.  Generally
 *	it has more in it than we need to have access too.
 *	So do this check here to see if it has been included
 *	too.
 */
#ifndef XWCMN_H

#ifdef UNDERSCORE

/*
 *	GEMLIB routines
 */

#ifndef gd_clos
#define gd_clos 	gd_clos_
#endif

#ifndef gg_init
#define gg_init		gg_init_
#endif 

#ifndef gg_maps
#define gg_maps		gg_maps_
#endif

#ifndef gg_motf
#define gg_motf         gg_motf_
#endif

#ifndef gg_sttl
#define gg_sttl		gg_sttl_
#endif 

#ifndef gg_panl
#define gg_panl		gg_panl_
#endif 

#ifndef gg_wstr
#define gg_wstr		gg_wstr_
#endif 

#ifndef im_btot
#define im_btot		im_btot_
#endif

#ifndef im_drop
#define im_drop		im_drop_
#endif

#ifndef im_lutf
#define im_lutf		im_lutf_
#endif

#ifndef im_rtbl
#define im_rtbl		im_rtbl_
#endif

#ifndef in_bdta
#define in_bdta		in_bdta_
#endif 

#ifndef in_text
#define in_text		in_text_
#endif 

#ifndef pr_tmkc
#define pr_tmkc		pr_tmkc_
#endif

#ifndef pr_tmkf
#define pr_tmkf         pr_tmkf_
#endif

/*
 *	GPLT routines
 */
/*
#ifndef gflush
#define gflush		gflush_
#endif 
*/

#ifndef gptvis
#define gptvis		gptvis_
#endif 

#ifndef gqbnd
#define gqbnd		gqbnd_
#endif

#ifndef gqcolr
#define gqcolr		gqcolr_
#endif

#ifndef gqmode
#define gqmode		gqmode_
#endif

#ifndef gqmprj
#define gqmprj		gqmprj_
#endif 

#ifndef gqsatn
#define gqsatn		gqsatn_
#endif 

#ifndef gqtext
#define gqtext		gqtext_
#endif 

#ifndef dstext
#define dstext		dstext_
#endif 

#ifndef gsatmg
#define gsatmg		gsatmg_
#endif 

#ifndef gscolr
#define gscolr		gscolr_
#endif 

#ifndef dscolr
#define dscolr		dscolr_
#endif 

#ifndef gsdev
#define gsdev		gsdev_
#endif 

#ifndef gsflnm
#define gsflnm		gsflnm_
#endif 

#ifndef gsicmn
#define gsicmn		gsicmn_
#endif 

#ifndef gsmprj
#define gsmprj		gsmprj_
#endif 

#ifndef gtrans
#define gtrans		gtrans_
#endif 

#ifndef xmotifw
#define xmotifw		xmotifw_
#endif

/*
 *	GARP routines
 */

#ifndef colorbardraw
#define colorbardraw	colorbardraw_
#endif

#ifndef countsfcobs
#define countsfcobs	countsfcobs_
#endif

#ifndef countupaobs
#define countupaobs	countupaobs_
#endif

#ifndef garperr
#define garperr		garperr_
#endif

#ifndef gdinfo
#define gdinfo 		gdinfo_
#endif

#ifndef getimgcomflt
#define getimgcomflt	getimgcomflt_
#endif

#ifndef getimgcomint
#define getimgcomint	getimgcomint_
#endif

#ifndef getimgcomstr
#define getimgcomstr	getimgcomstr_
#endif

#ifndef getnxtgrd
#define getnxtgrd	getnxtgrd_
#endif

#ifndef gfprintf
#define gfprintf	gfprintf_
#endif

#ifndef gfprinti
#define gfprinti	gfprinti_
#endif

#ifndef gfprints
#define gfprints	gfprints_
#endif

#ifndef gqsatm
#define gqsatm		gqsatm_
#endif 

#ifndef graphdraw
#define graphdraw	graphdraw_
#endif 

#ifndef gresize
#define gresize		gresize_
#endif 

#ifndef grid_close
#define grid_close	grid_close_
#endif 

#ifndef grid_nexttime
#define grid_nexttime	grid_nexttime_
#endif 

#ifndef grid_numtimes
#define grid_numtimes	grid_numtimes_
#endif 

#ifndef grid_open
#define grid_open	grid_open_
#endif 

#ifndef parse_gfunc
#define parse_gfunc	parse_gfunc_
#endif 

#ifndef pllgrid
#define pllgrid		pllgrid_
#endif 

#ifndef pbgmap
#define pbgmap		pbgmap_
#endif 

#ifndef pmap
#define pmap		pmap_
#endif

#ifndef prring
#define prring		prring_
#endif 

#ifndef psgrid
#define psgrid		psgrid_
#endif 

#ifndef psnprof
#define psnprof		psnprof_
#endif 

#ifndef pstream
#define pstream		pstream_
#endif

#ifndef psurf
#define psurf		psurf_
#endif 

#ifndef psym
#define psym		psym_
#endif 

#ifndef pthgt
#define pthgt		pthgt_
#endif

#ifndef ptitle
#define ptitle		ptitle_
#endif

#ifndef pupa
#define pupa		pupa_
#endif 

#ifndef pvgrid
#define pvgrid		pvgrid_
#endif 

#ifndef pvprof
#define pvprof		pvprof_
#endif

#ifndef pwprof
#define pwprof		pwprof_
#endif

#ifndef	pxsec
#define pxsec		pxsec_
#endif

#ifndef sfc_count
#define sfc_count	sfc_count_
#endif 

#ifndef sfc_list
#define sfc_list	sfc_list_
#endif 

#ifndef upa_count
#define upa_count	upa_count_
#endif 

#ifndef upa_list
#define upa_list	upa_list_
#endif 

#ifndef ti_ctoi
#define ti_ctoi		ti_ctoi_
#endif 

#ifndef ti_dayw
#define ti_dayw		ti_dayw_
#endif 


#endif  /* UNDERSCORE      */

#ifdef LINUX

#define grid_open_	grid_open__
#define grid_numtimes_	grid_numtimes__
#define grid_nexttime_	grid_nexttime__
#define grid_close_	grid_close__

#define in_text_	in_text__
#define gg_sttl_	gg_sttl__
#define gg_wstr_	gg_wstr__
#define parse_gfunc_	parse_gfunc__

#define ti_ctoi_	ti_ctoi__
#define ti_dayw_	ti_dayw__
#define im_btot_	im_btot__
#define im_rtbl_	im_rtbl__
#define pr_tmkc_	pr_tmkc__
#define pr_tmkf_	pr_tmkf__
#define gg_motf_	gg_motf__

#define sfc_count_	sfc_count__
#define sfc_list_	sfc_list__
#define upa_count_	upa_count__
#define upa_list_	upa_list__

#endif


#endif  /*  XWCMN_H        */

#endif  /*  __UNDERSCORE_H  */
