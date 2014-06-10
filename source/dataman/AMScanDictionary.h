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


#ifndef AMSCANDICTIONARY_H
#define AMSCANDICTIONARY_H

#include "dataman/AMScanParametersDictionary.h"

class AMScan;

class AMScanDictionary : public AMOldScanParametersDictionary
{
	Q_OBJECT
public:
 	virtual ~AMScanDictionary();
	AMScanDictionary(AMScan *scan, QObject *parent = 0);

	bool canOperateOnName() const;
	bool canOperateOnExportName() const;
	bool canOperate() const;

public slots:
	void setOperatingOnName(bool operatingOnName);
	void setOperatingOnExportName(bool operatingOnExportName);

protected:
	/// Initializes the keywordDictionary_ with the functions you see below. You can always add more directly to keywordDictionary_.
	void loadKeywordReplacementDictionaryImplementation();
	void operateImplementation(const QString &input);

	///////////////////////////////
	// functions to implement the keyword replacement system
	///////////////////////////////

	QString krName(const QString& arg = QString());
	QString krTechnique(const QString &arg = QString());
	QString krNumber(const QString& arg = QString());
	QString krNotes(const QString& arg = QString());
	QString krDate(const QString& arg = QString());
	QString krTime(const QString& arg = QString());
	QString krDateTime(const QString& arg = QString());

	QString krRun(const QString& arg = QString());
	QString krRunName(const QString& arg = QString());
	QString krRunStartDate(const QString& arg = QString());
	QString krRunEndDate(const QString& arg = QString());
	QString krRunNotes(const QString& arg = QString());

	QString krFacilityName(const QString& arg = QString());
	QString krFacilityDescription(const QString& arg = QString());

	QString krScanConfiguration(const QString& propertyName);

	QString krControl(const QString& controlName);
	QString krControlName(const QString &controlName);
	QString krControlDescription(const QString& controlName);
	QString krControlValue(const QString& controlName);
	QString krControlUnits(const QString& controlName);

	QString krAllControls(const QString& arg = QString());

	QString krSample(const QString& arg = QString());
	QString krSampleName(const QString& arg = QString());
	QString krSampleElements(const QString& arg = QString());
	QString krSampleCreationDate(const QString& arg = QString());
	QString krSampleNotes(const QString& arg = QString());

protected:
	/// The scan this dictionary is going to operate on
	AMScan *scan_;
};

#endif // AMSCANDICTIONARY_H
