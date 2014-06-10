/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSTATUSVIEW_H
#define AMSTATUSVIEW_H

#include <QWidget>
#include <QAbstractButton>
#include <QLabel>
#include <QCheckBox>
#include <QTextEdit>
#include <QBoxLayout>
#include <QStandardItemModel>
#include <QTableView>

#include "util/AMErrorMonitor.h"
#include "util/AMDeferredFunctionCall.h"

class QSystemTrayIcon;

/// This widget can be used to keep a log (or history) of AMErrorReport messages in a table view.  It's opened by double-clicking on an AMStatusView.
class AMStatusLogView : public QWidget {
	Q_OBJECT

public:
 	virtual ~AMStatusLogView();
	explicit AMStatusLogView(QWidget* parent = 0);

public slots:
	void addError(const AMErrorReport& e);

protected slots:
	/// Handles the toggling of AMErrorMon debugging messages
	void onEnableDebuggingCheckBoxStateChanged(int state);

protected:
	QStandardItemModel* logModel_;
	QTableView* logView_;

	/// Check box to enable and disable the debugging from AMErrorMon
	QCheckBox *enableDebuggingCheckBox_;

	QIcon debugIcon_, alertIcon_, infoIcon_, seriousIcon_;
};

/// This widget shows the last error/status message in a label, and displays a larger textview with a history of messages when you mouse-over it.
class AMStatusView : public QAbstractButton
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMStatusView();
	explicit AMStatusView(QWidget *parent = 0);

signals:

public slots:
	/// Handles any errors that are logged using AMErrorMon::report(AMErrorCode())
	void onAnyError(AMErrorReport e);

protected slots:
	/// Handles calling the system tray icon messages
	void handleSystemTrayIconRequests();

protected:
	virtual void paintEvent(QPaintEvent *e) {
		QWidget::paintEvent(e);
	}

protected:
	/// Icons representing the nature of the last notification
	QLabel *iconInfo_, *iconAlert_, *iconSerious_, *iconDebug_, *currentIcon_;
	/// last notification text
	QLabel* shortText_;

	/// layout
	QHBoxLayout* hl_;

	/// A widget to display a log of recent events
	AMStatusLogView* logView_;

	/// SystemTrayIcon object used to display error notifications very visibly on-screen.
	QSystemTrayIcon* sicon_;

	/// Deferred call for handling the actual system tray icon calls
	AMDeferredFunctionCall systemTrayIconFunctionCall_;
	/// List of deferred messages to report
	QStringList messagesToReport_;
};

#endif // AMSTATUSVIEW_H
