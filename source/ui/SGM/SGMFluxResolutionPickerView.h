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


#ifndef SGMFLUXRESOLUTIONPICKERVIEW_H
#define SGMFLUXRESOLUTIONPICKERVIEW_H

#include <QGroupBox>
#include "dataman/info/AMControlInfoList.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class AMExtendedControlEditor;
class AMScanAxis;

class SGMFluxResolutionPickerStaticView : public QGroupBox{
Q_OBJECT
public:
	virtual ~SGMFluxResolutionPickerStaticView();
	SGMFluxResolutionPickerStaticView(AMScanAxis *scanAxis, QWidget *parent = 0);

public slots:
	virtual void setFromInfoList(const AMControlInfoList &infoList);

protected:
	AMScanAxis *scanAxis_;
	double minEnergy_;
	double maxEnergy_;

	QGroupBox *exitSlitGapGroupBox_;
	QLineEdit *exitSlitGapLineEdit_;
	QGroupBox *gratingGroupBox_;
	QLineEdit *gratingLineEdit_;
	QGroupBox *harmonicGroupBox_;
	QLineEdit *harmonicLineEdit_;

	QVBoxLayout *buttonsVL_;
	QVBoxLayout *ceVL_;
	QHBoxLayout *settingsHL_;
	QVBoxLayout *mainVL_;
};

class SGMFluxResolutionPickerView : public SGMFluxResolutionPickerStaticView
{
Q_OBJECT
public:
	virtual ~SGMFluxResolutionPickerView();
	SGMFluxResolutionPickerView(AMScanAxis *scanAxis, QWidget *parent = 0);

public slots:
	virtual void setFromInfoList(const AMControlInfoList &infoList);

	void setDisabled(bool disabled);

protected slots:
	void onRegionsChanged();
	void onSetpointsChanged();

	void onBestFluxButtonClicked();
	void onBestResolutionButtonClicked();

signals:
	void configValuesChanged(AMControlInfoList);

protected:
	QPushButton *bestFluxButton_;
	QPushButton *bestResolutionButton_;
	AMExtendedControlEditor *exitSlitGapCE_;
	AMExtendedControlEditor *gratingCE_;
	AMExtendedControlEditor *harmonicCE_;
	QLabel *warningsLabel_;
};

#endif // SGMFLUXRESOLUTIONPICKERVIEW_H
