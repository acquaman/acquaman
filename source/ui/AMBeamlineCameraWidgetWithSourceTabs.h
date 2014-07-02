/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMBEAMLINECAMERAWIDGETWITHSOURCETABS_H
#define AMBEAMLINECAMERAWIDGETWITHSOURCETABS_H

#include "ui/AMBeamlineCameraWidget.h"

#include <QList>
#include <QUrl>

class QTabBar;

/// This widget extends AMBeamlineCameraWidget with a row of tabs to let the user choose from different camera views.
class AMBeamlineCameraWidgetWithSourceTabs : public AMBeamlineCameraWidget
{
    Q_OBJECT
public:
	/// Constructor. If \c initialSource is a valid URL, an initial tab and source is created.
 	virtual ~AMBeamlineCameraWidgetWithSourceTabs();
	AMBeamlineCameraWidgetWithSourceTabs(const QUrl& initialSource = QUrl(), const QString& initialDescription = QString(), QWidget *parent = 0, bool useOpenGl = true);

	/// Adds a source. If it's the first source, we'll start playing it.
	void addSource(const QString& description, const QUrl& sourceUrl);

	/// Returns the number of sources
	int sourceCount() const { return sourceUrls_.count(); }
	/// Returns the URL of the source at \c index
	QUrl sourceAt(int index) const { if(index<0 || index>=sourceCount()) return QUrl(); return sourceUrls_.at(index); }


public slots:
	/// Start playing the source at a \c index
	void playSourceAt(int index) { playSource(sourceAt(index)); }
	/// Removes the tab and source at \c index
	void removeSource(int index);

signals:

public slots:

protected:
	QTabBar* tabBar_;
	QList<QUrl> sourceUrls_;
};

#endif // AMBEAMLINECAMERAWIDGETWITHSOURCETABS_H
