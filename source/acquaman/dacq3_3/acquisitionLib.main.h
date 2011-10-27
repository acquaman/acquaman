/*
 * $Header: acquisitionLib.main.h 1.4.1.7 2009/03/04 15:13:45CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *    Version numbering.
 */

#if defined(ACQLIB_VERSION)

#undef _V_MAJOR_
#undef _V_MINOR_
#define _V_MAJOR_ (ACQLIB_VERSION/10000)
#define _V_MINOR_ ((ACQLIB_VERSION/100)%100)

#if _V_MAJOR_ == 0

#elif _V_MAJOR_ == 1
#	if _V_MINOR_==0
#	include <acquisitionLib.1.0.h>
#	elif _V_MINOR_==1
#	include <acquisitionLib.1.1.h>
#	elif _V_MINOR_==2
#	include <acquisitionLib.1.2.h>
#	endif

#elif _V_MAJOR_ == 2
#	if _V_MINOR_==0
#	include <acquisitionLib.2.0.h>
#	elif _V_MINOR_==1
#	include <acquisitionLib.2.1.h>
#	elif _V_MINOR_==2
#	include <acquisitionLib.2.2.h>
#	elif _V_MINOR_==4
#	include <acquisitionLib.2.4.h>
#	elif _V_MINOR_==5
#	include <acquisitionLib.2.5.h>
#	elif _V_MINOR_==6
#	include <acquisitionLib.2.6.h>
#	elif _V_MINOR_==7
#	include <acquisitionLib.2.7.h>
#	elif _V_MINOR_==8
#	include <acquisitionLib.2.8.h>
#	endif


#elif _V_MAJOR_ == 3
#	if _V_MINOR_==0
#	include <acquisitionLib.3.0/acquisitionLib.h>
#	elif _V_MINOR_==1
#	include <acquisitionLib.3.1/acquisitionLib.h>
#	elif _V_MINOR_==2
#	include <acquisitionLib.3.2/acquisitionLib.h>
#	elif _V_MINOR_==4
#	include <acquisitionLib.3.4/acquisitionLib.h>
#	endif

#endif

#endif /* ACQLIB_VERSION */

/* if no inclusion occured, provide a default entry */
//#if !defined(ACQUISITIONLIB_H)
#ifndef DACQLIB_ACQUISITIONLIB_H
#if defined(ACQLIB_VERSION)
#warning "Unknown acquisition library version number, using default"
#endif
#include <acquisitionLib.3.0/acquisitionLib.h>
#endif

/*
 * $Log: acquisitionLib.main.h  $
 * Revision 1.4.1.7 2009/03/04 15:13:45CST Glen Wright (wrightg) 
 * Revisions for caller-defined output streams
 * Revision 1.4.1.5 2008/05/28 14:34:57CST Glen Wright (wrightg) 
 * Fixed reference to default library
 * Revision 1.4.1.4 2008/05/28 14:28:20CST Glen Wright (wrightg) 
 * Unclosed comment causes no default loading
 * Revision 1.4.1.3 2008/05/28 13:53:02CST Glen Wright (wrightg) 
 * Initial 3.0 version
 * Revision 1.4.1.2 2007/08/21 13:00:16CST Glen Wright (wrightg) 
 * intermediate check-in so I can edit this file in the main stream.
 * Revision 1.4.1.1 2007/03/18 15:06:49CST Glen Wright (wrightg) 
 * Duplicate revision
 */

