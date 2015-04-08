
DESTDIR = build

HEADERS *=\
	source/dataman/SGM/SGM2004FileLoader.h \
	source/dataman/SGM/SGM2010FastFileLoader.h \
	source/dataman/SGM/SGMMCPDetectorInfo.h \
	source/dataman/SGM/SGM2011XASFileLoader.h \
	source/dataman/export/SGM/SGMAxis2000Exporter.h \
	source/dataman/REIXS/REIXSXESCalibration2.h \
	source/dataman/VESPERS/VESPERSXASDataLoader.h
SOURCES *= \
	source/dataman/SGM/SGM2004FileLoader.cpp \
	source/dataman/SGM/SGM2010FastFileLoader.cpp \
	source/dataman/SGM/SGM2011XASFileLoader.cpp \
	source/dataman/SGM/SGMMCPDetectorInfo.cpp \
	source/dataman/export/SGM/SGMAxis2000Exporter.cpp \
	source/dataman/REIXS/REIXSXESRawFileLoader.cpp \
	source/dataman/REIXS/REIXSXESCalibration2.cpp \
	source/dataman/VESPERS/VESPERSXASDataLoader.cpp
