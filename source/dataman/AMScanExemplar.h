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


#ifndef AMSCANEXEMPLAR_H
#define AMSCANEXEMPLAR_H

#include <QObject>
#include <QDateTime>

class AMScanConfiguration;

class AMScanExemplar : public QObject
{
Q_OBJECT
public:
 	virtual ~AMScanExemplar();
	AMScanExemplar(QObject *parent = 0);

	QString name() const;
	QString technique() const;
	QString number() const;
	QDateTime dateTime() const;
	QString runName() const;
	QDateTime runStartDateTime() const;
	QDateTime runEndDateTime() const;
	QString facilityName() const;
	QString facilityDescription() const;
	const AMScanConfiguration* scanConfiguration() const;
	QString sampleName() const;
	QString sampleElements() const;
	QDateTime sampleDateTime() const;
	QString exportName() const;

	AMScanExemplar& operator=(const AMScanExemplar &other);

public slots:
	void setName(const QString &name);
	void setDateTime(const QDateTime &dateTime);
	void setRunName(const QString &runName);
	void setRunStartDateTime(const QDateTime &runStartDateTime);
	void setRunEndDateTime(const QDateTime &runEndDateTime);
	void setFacilityName(const QString &facilityName);
	void setFacilityDescription(const QString &facilityDescription);
	void setScanConfiguration(const AMScanConfiguration *scanConfiguration);
	void setSampleName(const QString &sampleName);
	void setSampleElements(const QString &sampleElements);
	void setSampleDateTime(const QDateTime &sampleDateTime);
	void setExportName(const QString &exportName);

signals:
	void settingsChanged();

protected:
	QString name_;
	QString number_;
	QDateTime dateTime_;
	QString runName_;
	QDateTime runStartDateTime_;
	QDateTime runEndDateTime_;
	QString facilityName_;
	QString facilityDescription_;
	AMScanConfiguration const *scanConfiguration_;
	QString sampleName_;
	QString sampleElements_;
	QDateTime sampleDateTime_;
	QString exportName_;
};

#endif // AMSCANEXEMPLAR_H
