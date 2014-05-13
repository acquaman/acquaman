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


#ifndef AMSCANEXEMPLARDICTIONARY_H
#define AMSCANEXEMPLARDICTIONARY_H

#include "dataman/AMScanParametersDictionary.h"

class AMScanConfiguration;
class AMScanExemplar;

/// Can operate on name and exportName (but assumes $exportName can't be used in $name in addition to normal circular references)

class AMScanExemplarDictionary : public AMOldScanParametersDictionary
{
Q_OBJECT
public:
 	virtual ~AMScanExemplarDictionary();
	AMScanExemplarDictionary(AMScanExemplar *exemplar, QObject *parent = 0);

	bool canOperateOnName() const;
	bool canOperateOnExportName() const;
	bool canOperate() const;

public slots:
	void setOperatingOnName(bool operatingOnName);
	void setOperatingOnExportName(bool operatingOnExportName);

protected:
	void loadKeywordReplacementDictionaryImplementation();
	void operateImplementation(const QString &input);

	QString krName(const QString& arg = QString());
	QString krTechnique(const QString &arg = QString());
	QString krNumber(const QString& arg = QString());
	QString krDate(const QString& arg = QString());
	QString krTime(const QString& arg = QString());
	QString krDateTime(const QString& arg = QString());

	QString krRun(const QString& arg = QString());
	QString krRunName(const QString& arg = QString());
	QString krRunStartDate(const QString& arg = QString());
	QString krRunEndDate(const QString& arg = QString());

	QString krFacilityName(const QString& arg = QString());
	QString krFacilityDescription(const QString& arg = QString());

	QString krScanConfiguration(const QString& propertyName);

	QString krSample(const QString& arg = QString());
	QString krSampleName(const QString& arg = QString());
	QString krSampleElements(const QString& arg = QString());
	QString krSampleCreationDate(const QString& arg = QString());

	QString krExportName(const QString &arg = QString());

protected:
	AMScanExemplar *exemplar_;
};

#endif // AMSCANEXEMPLARDICTIONARY_H
