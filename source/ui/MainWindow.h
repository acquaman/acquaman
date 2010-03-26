#ifndef ACQMAN_MainWindow_H
#define ACQMAN_MainWindow_H

#include <QtGui>
#include "Sidebar.h"

#include "ConnectionSettings.h"
#include "SamplePositions.h"
#include "GratingResolution.h"
#include "AbsorptionScanController.h"
#include "EmissionScanController.h"
#include "Scheduler.h"
#include "PeriodicTable.h"
#include "ProtocolViewer.h"
#include "ExpAlbum.h"
#include "BottomBar.h"

class MainWindow : public QWidget {
	
	Q_OBJECT
	
public:
	MainWindow(QWidget* parent = 0);
	~MainWindow() {};


protected:
	
	Sidebar* sidebar_;
	BottomBar* bottomBar_;
	
	QStackedWidget* stackWidget_;
	
	
	// Main full-screen widgets:
	ConnectionSettings* connectionSettings_;
	SamplePositions* samplePositions_;
	GratingResolution* gratingResolution_;
	AbsorptionScanController* absorptionScanController_;
	EmissionScanController* emissionScanController_;
	Scheduler* scheduler_;
	PeriodicTable* periodicTable_;
	ProtocolViewer* protocolViewer_;
	ExpAlbum* expAlbum_;
	
	
};


#endif
