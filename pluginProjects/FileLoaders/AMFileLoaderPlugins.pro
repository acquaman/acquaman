# #####################################################################
# QMake project file for acquaman file loaders			October 2014. david.k.chevrier@gmail.com
# #####################################################################

TEMPLATE = subdirs

SUBDIRS += AMCDFv1FileLoader \
	SGM2011XASFileLoader \
	SGM2004XASFileLoader \
	SGM2010FastFileLoader \
	VESPERS2011XASFileLoader \
	VESPERS2011XRFFileLoader \
	REIXSXESRawFileLoader \
	ALSBL8XASFileLoader \
	VESPERS20122DFileLoader \
	VESPERS2012SpatialLineScanFileLoader \
	AMRegion2013FileLoader \
	SGM2013FastFileLoader
