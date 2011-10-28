/* $Header: epicsConnectApp/src/epicsConnect.main.h 1.2 2008/07/16 16:50:07CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *
 */

#if defined(EPICSCONNECT_VERSION)

#undef __V_MAJOR__
#undef __V_MINOR__
#define __V_MAJOR__ (EPICSCONNECT_VERSION/10000)
#define __V_MINOR__ ((EPICSCONNECT_VERSION/100)%100)
#if __V_MAJOR__ == 0

#elif __V_MAJOR__ == 1
#	if __V_MINOR__==1
#	include <epicsConnect.1.1.h>
#	elif __V_MINOR__ == 2
#	include <epicsConnect.1.2.h>
#	elif __V_MINOR__ == 3
#	include <epicsConnect.1.3.h>
#	elif __V_MINOR__ == 4
#	include <epicsConnect.1.4.h>
#	elif __V_MINOR__ == 5
#	include <epicsConnect.1.5.h>
#	endif

#elif __V_MAJOR__ == 2
#	if __V_MINOR__==0
#	include <epicsConnect.2.0.h>

#	elif __V_MINOR__==1
#	include <epicsConnect.2.1.h>

#	endif

#endif

#else
#include <epicsConnect.1.4.h>
#endif /* EPICSCONNECT_VERSION */

/* 
 * $Log: epicsConnectApp/src/epicsConnect.main.h  $
 * Revision 1.2 2008/07/16 16:50:07CST Glen Wright (wrightg) 
 * 
 * Revision 1.1 2008/05/08 10:19:20CST David Beauregard (beaured0) 
 * Initial revision
 * Member added to project e:/MKS_Home/archive/cs/epics_local/epics_tools/epicsConnect/project.pj
 * Revision 1.6 2007/11/14 16:52:25CST Glen Wright (wrightg) 
 * Updates according to change package; Changed library versioning to easily follow 3-point convention
 * Revision 1.5 2007/05/19 12:40:01CST Elder Matias (matiase) 
 * added header/log
 */
