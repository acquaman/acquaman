/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMDETECTORVIEW_H
#define AMDETECTORVIEW_H

#include "AMControlSetView.h"
#include "AMDetectorViewSupport.h"
#include "beamline/AMSingleControlDetector.h"
#include "beamline/MCPDetector.h"
#include "beamline/PGTDetector.h"
#include "ui/AMControlEditor.h"
#include "ui/AMDetectorInfoView.h"
#include "QMessageBox"
#include "QMetaMethod"

class AMDetectorView : public QWidget
{
Q_OBJECT

public:
	Q_INVOKABLE explicit AMDetectorView(bool configureOnly = false, QWidget *parent = 0);

	virtual AMDetector* detector();

	virtual AMDetectorInfo* configurationSettings() const;

public slots:

signals:
	void settingsChangeRequested();
	void settingsConfigureRequested();

protected:
	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	virtual bool setDetector(AMDetector *detector, bool configureOnly = false);
	friend AMDetectorView* AMDetectorViewSupport::createDetectorView(AMDetector *detector, bool configureOnly);
	friend AMDetectorView* AMDetectorViewSupport::createBriefDetectorView(AMDetector *detector, bool configureOnly);
	friend AMDetectorView* AMDetectorViewSupport::createDetailedDetectorView(AMDetector *detector, bool configureOnly);

protected:
	bool configureOnly_;
};

class AMBriefDetectorView : public AMDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMBriefDetectorView(bool configureOnly = false, QWidget *parent = 0);

protected:
	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	virtual bool setDetector(AMDetector *detector, bool configureOnly = false);
};

class AMDetailedDetectorView : public AMDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDetailedDetectorView(bool configureOnly = false, QWidget *parent = 0);

protected:
	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	virtual bool setDetector(AMDetector *detector, bool configureOnly = false);
};

class PGTOldDetectorView : public PGTOldDetectorInfoView
{
	Q_OBJECT
public:
	PGTOldDetectorView(PGTDetector *detector, AMDetectorInfo *configDetector = 0, bool editMode = false, QWidget *parent = 0);

protected slots:
	void onIntegrationModeUpdate(double value);
	void onIntegrationModeChange(int index);
	void setEditMode(bool editMode);
	void setEditable();

protected:
	PGTDetector *detector_;
	bool editMode_;
	QDoubleSpinBox *integrationTimeFbk_;
	QComboBox *integrationModeFbk_;
	QDoubleSpinBox *hvFbk_;
};

class MCPOldDetectorView : public MCPOldDetectorInfoView
{
	Q_OBJECT
public:
	MCPOldDetectorView(MCPDetector *detector, AMDetectorInfo *configDetector = 0, bool editMode = false, QWidget *parent = 0);

protected slots:
	void setEditMode(bool editMode);
	void setEditable();

protected:
	MCPDetector *detector_;
	bool editMode_;
	QDoubleSpinBox *hvFbk_;
};

class AMOldDetectorSetView : public AMDetectorInfoSetView
{
	Q_OBJECT
public:
	AMOldDetectorSetView(AMOldDetectorInfoSet *viewSet, AMOldDetectorInfoSet *configSet = 0, bool setup = true, QWidget *parent = 0);

public slots:
	void setEditMode(bool editMode);
	void setEditable();

protected:
	bool editMode_;

	virtual void runSetup();
	virtual QWidget* detailViewByType(AMDetectorInfo *detector, AMDetectorInfo *configDetector);
};


#endif // AMDETECTORVIEW_H
