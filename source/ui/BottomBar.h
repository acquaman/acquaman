/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ACQMAN_BOTTOMBAR_H
#define ACQMAN_BOTTOMBAR_H

#include <QtGui/QWidget>
#include <QTime>
#include <QDebug>
#include "ui_BottomBar.h"
#include "AMStatusView.h"

class BottomBar : public QWidget, private Ui::BottomBarClass {
	Q_OBJECT

public:
	BottomBar(QWidget *parent = 0);

	~BottomBar();

public slots:
	void updateScanProgress(double secondsElapsed, double secondsTotal);
	void enableScanInProgressButtons() {}		// enables restartScan, stopScan, pauseScan

protected:

	AMStatusView* statusView_;

	void makeConnections();	// TODO


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

	void addButtonClicked();
};

#endif // BOTTOMBAR_H

/*
QWidget#BottomBarClass {
border: 1px solid black;
border-top-color: white;
background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(202, 202, 202, 255), stop:0.494444 rgba(191, 191, 191, 255), stop:0.5 rgba(179, 179, 179, 255), stop:1 rgba(179, 179, 179, 255));
}*/
