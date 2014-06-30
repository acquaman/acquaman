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


#ifndef AMSAMPLECAMERABROWSER_H
#define AMSAMPLECAMERABROWSER_H

#include <QObject>

class QString;
class QColor;
class QPointF;
class QStringList;
class AMSampleCamera;
class AMSampleCameraView;
class AMSamplePlate;

#include "dataman/database/AMDbObject.h"

class AMSampleCameraURL : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(QString urlString READ urlString WRITE setURLString)

public:
	Q_INVOKABLE AMSampleCameraURL(const QString &urlString = QString(), QObject *parent = 0);

	QString urlString() const;

public slots:
	void setURLString(const QString &urlString);

protected:
	QString urlString_;
};

class AMSampleCameraBrowser : public QObject
{
	Q_OBJECT
public:
	AMSampleCameraBrowser(QObject *parent = 0);
	/// Accessors
	QString currentURL();
	AMSampleCamera *shapeDataSet();

	/// Mutators
	void setCurrentURL(QString currentURL);

	void addURL(const QString &urlString);
	void removeURL(const QString &urlString);

	QList<AMSampleCameraURL*> urls();

protected slots:
	void onSamplePlateChanged(AMSamplePlate *samplePlate);
	void onCameraShapesChanged();

protected:
	AMSampleCamera *shapeDataSet_;
	AMSamplePlate *currentSamplePlate_;

	QString currentURL_;
	QList<AMSampleCameraURL*> urls_;
};

#endif // AMSAMPLECAMERABROWSER_H
