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


#ifndef AMSINGLECONTROLDETECTOR_H
#define AMSINGLECONTROLDETECTOR_H

#include "AMOldDetector.h"
#include "AMPVControl.h"

class AMSingleControlDetector : public AMOldDetectorInfo, public AMOldDetector
{
Q_OBJECT
public:
	AMSingleControlDetector(const QString& name, AMControl *control, AMOldDetector::ReadMethod readMethod = AMOldDetector::ImmediateRead, QObject *parent = 0);
	~AMSingleControlDetector();

	const QMetaObject* getMetaObject();

	virtual double reading() const;


	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMOldDetectorInfo* toInfo() const;

	AMControl* control();

	/* NTBA March 14, 2011 David Chevrier
	bool setFromInfo(const AMDetectorInfo &info);
	*/
	bool setFromInfo(const AMOldDetectorInfo *info);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(AMOldDetectorInfo *detectorSettings);

protected slots:
	void onControlConnected(bool connected);

protected:
	AMControl *control_;
};

class AMSingleReadOnlyControlDetector : public AMOldDetectorInfo, public AMOldDetector
{
Q_OBJECT
public:
	AMSingleReadOnlyControlDetector(const QString& name, const QString &baseName, AMOldDetector::ReadMethod readMethod = AMOldDetector::ImmediateRead, QObject *parent = 0);
	~AMSingleReadOnlyControlDetector();

	const QMetaObject* getMetaObject();

	virtual QString dacqName() const;

	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMOldDetectorInfo* toInfo() const;

	AMControl* control();

	bool setFromInfo(const AMOldDetectorInfo *info);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(AMOldDetectorInfo *detectorSettings);

protected slots:
	void onControlConnected(bool connected);

protected:
	AMControl *control_;
};

#endif // AMSINGLECONTROLDETECTOR_H
