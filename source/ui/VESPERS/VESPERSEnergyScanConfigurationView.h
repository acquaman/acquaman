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


#ifndef VESPERSENERGYSCANCONFIGURATIONVIEW_H
#define VESPERSENERGYSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "ui/acquaman/AMRegionsView.h"
#include "acquaman/VESPERS/VESPERSEnergyDacqScanController.h"
#include "acquaman/VESPERS/VESPERSEnergyScanConfiguration.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>

/// This class builds the view for configuring a CCD energy scan for the VESPERS beamline.
class VESPERSEnergyScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.  Passes in the configuration that this view will modify.
	VESPERSEnergyScanConfigurationView(VESPERSEnergyScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration *configuration() const { return config_; }

public slots:
	/// Slot that switches which sample stage is viewed inside the persitent view.  True is pseudo motors, false is real motors.
	void setSampleStage(bool sampleStage);

signals:
	/// Sends out a request that the current detector (based on FluorescenceDetectorChoice) to be configured.  Asks the app controller to change to the detector view.  String will be either "Single Element" or "Four Element".
	void configureDetector(const QString &);

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited() { config_->setName(scanName_->text()); config_->setUserScanName(scanName_->text()); }

	/// Handles changes to the ccd detector choice.
	void onCCDDetectorChanged(int id) { config_->setCCDDetector(id); }
	/// Handles propgating changes in the config to the detector buttons.
	void updateCCDDetectorButtons(int detector);
	/// Sets the current horizontal and vertical positions and saves them in the configuration.
	void setScanPosition()
	{
		config_->setPosition(xPosition_->value(), yPosition_->value());
		savedXPosition_->setText(QString::number(config_->x(), 'g', 3) + " mm");
		savedYPosition_->setText(QString::number(config_->y(), 'g', 3) + " mm");
		positionsSaved_->setText("Saved");
		QPalette palette(this->palette());
		palette.setColor(QPalette::Active, QPalette::WindowText, Qt::darkGreen);
		positionsSaved_->setPalette(palette);
		connect(xPosition_, SIGNAL(valueChanged(double)), this, SLOT(onXorYPositionChanged()));
		connect(yPosition_, SIGNAL(valueChanged(double)), this, SLOT(onXorYPositionChanged()));
	}
	/// Helper slot.  If the value is changed from either of the x or y position spin boxes then the positions saved label should change to unsaved.
	void onXorYPositionChanged()
	{
		disconnect(xPosition_, SIGNAL(valueChanged(double)), this, SLOT(onXorYPositionChanged()));
		disconnect(yPosition_, SIGNAL(valueChanged(double)), this, SLOT(onXorYPositionChanged()));

		positionsSaved_->setText("Unsaved");
		QPalette palette(this->palette());
		palette.setColor(QPalette::Active, QPalette::WindowText, Qt::darkRed);
		positionsSaved_->setPalette(palette);
	}
	/// Helper slot that sets the time offset for the scan.
	void setTimeOffset(double time) { config_->setTimeOffset(time); }
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();

	/// Handles changes in the name of the CCD file name and sets the label that corresponds to it.
	void onCCDFileNameChanged(QString name) { currentCCDFileName_->setText(QString("Current CCD file name:\t%1").arg(name)); }
	/// Emits the configureDetector signal based with 'Roper CCD' or 'Mar CCD.
	void onConfigureCCDDetectorClicked();

	/// Handles the context menu.
	void onCustomContextMenuRequested(QPoint pos);

protected:
	/// Helper method that takes a time in seconds and returns a string of d:h:m:s.
	QString convertTimeToString(double time);

	/// Pointer to the specific scan config the view is modifying.
	VESPERSEnergyScanConfiguration *config_;

	/// This lets you setup regions.
	AMRegionsView *regionsView_;

	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Pointer to the label holding the current file name.
	QLabel *currentCCDFileName_;

	/// The spin box that holds the x coordinate for the scan position.
	QDoubleSpinBox *xPosition_;
	/// The spin box htat holds the y coordinate for the scan position.
	QDoubleSpinBox *yPosition_;
	/// Label holding what the currently saved x position is in the scan configuration.
	QLabel *savedXPosition_;
	/// Label holding what the currently saved y position is in the scan configuration.
	QLabel *savedYPosition_;
	/// Label holding whether or not the x and y positions have been saved yet.
	QLabel *positionsSaved_;

	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;

	/// Button group for the ccd detector selection.
	QButtonGroup *ccdButtonGroup_;

	/// A label holding text for the the time offset spin box.
	QLabel *timeOffsetLabel_;
	/// A spin box holding the time offset.
	QDoubleSpinBox *timeOffset_;
};

#endif // VESPERSENERGYSCANCONFIGURATIONVIEW_H
