#ifndef BOTTOMBAR_H
#define BOTTOMBAR_H

#include <QtGui/QWidget>
#include "ui_BottomBar.h"


class BottomBar : public QWidget, private Ui::BottomBarClass {
    Q_OBJECT

public:
    BottomBar(QWidget *parent = 0);
    ~BottomBar();

public slots:
	void updateScanProgress(int /*secondsElapsed*/, int /*secondsTotal*/) {}
	void enableScanInProgressButtons() {}		// enables restartScan, stopScan, pauseScan
	
protected:
	void makeConnections() {}	// TODO
	
protected slots:
    // TODO
    
 signals:
 	void searchIssued(const QString&);
 	void restartScanIssued();
 	void stopScanIssued();
 	void pauseScanIssued();
 	void continueScanIssued();
 	void fullScreenIssued();
 	void adjustFinishPointsIssued();
};

#endif // BOTTOMBAR_H

/*
QWidget#BottomBarClass {
border: 1px solid black;
border-top-color: white;
background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(202, 202, 202, 255), stop:0.494444 rgba(191, 191, 191, 255), stop:0.5 rgba(179, 179, 179, 255), stop:1 rgba(179, 179, 179, 255));
}*/
