#ifndef VESPERSROPERCCDDETECTORVIEW_H
#define VESPERSROPERCCDDETECTORVIEW_H

#include "ui/beamline/AMDetectorView.h"
#include "beamline/VESPERS/VESPERSRoperCCDDetector.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>

class VESPERSRoperCCDDetectorView : public AMDetailedDetectorView
{
	Q_OBJECT

public:
	/// Default contructor.  Can build a view with a null pointer (ie: not a useful view) if necessary.
	Q_INVOKABLE explicit VESPERSRoperCCDDetectorView(VESPERSRoperCCDDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns a pointer to the detector being viewed.
	AMDetector *detector() { return detector_; }

protected slots:
	/// Slot used to switch the icon on the currently acquiring indicator.
	void onIsAcquiringChanged(bool isAcquiring) { isAcquiring_->setPixmap(QIcon(isAcquiring == true ? ":/ON.png" : ":/OFF.png").pixmap(25)); }
	/// Slot used to update the time remaining label.
	void onTimeRemainingChanged(double time) { timeRemaining_->setText(QString::number(time, 'g', 2)+" s"); }
	/// Slot used to set the acquire time on the detector.
	void setAcquireTime(double time) { if (time != detector_->acquireTime()) detector_->setAcquireTime(time); }
	/// Slot used to update the trigger mode combo box.
	void onTriggerModeChanged(VESPERSRoperCCDDetector::TriggerMode mode);
	/// Slot used to set the trigger mode on the detector.
	void setTriggerMode(int newMode);
	/// Slot used to update the image mode combo box.
	void onImageModeChanged(VESPERSRoperCCDDetector::ImageMode mode);
	/// Slot used to set the image mode on the detector.
	void setImageMode(int newMode);
	/// Used to set the CCD Path when it changes from the program.
	void ccdPathEdited()
	{
		if (filePathEdit_->text().at(filePathEdit_->text().size()-1) != '\\')
			filePathEdit_->setText(filePathEdit_->text()+"\\");

		endstation_->setCCDPath(filePathEdit_->text());
	}
	/// Used to set the CCD File name when it changes from the program.
	void ccdFileEdited() { endstation_->setCCDName(fileNameEdit_->text()); }
	/// Used to set the CCD Number when it changes from the program.
	void ccdNumberEdited() { endstation_->setCCDNumber(fileNumberEdit_->text().toInt()); }
	/// Used to update the ccdNumber value.
	void ccdNumberUpdate(int val) { fileNumberEdit_->setText(QString::number(val)); }

protected:
	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMDetector *detector, bool configureOnly);

	/// The pointer to the detector.
	VESPERSRoperCCDDetector *detector_;

	/// Label holding the isAcquiring state.
	QLabel *isAcquiring_;
	/// Label holding the current time remaining.
	QLabel *timeRemaining_;
	/// Spin box holding the acquire time.
	QDoubleSpinBox *acquireTime_;
	/// Combo box holding the trigger mode.
	QComboBox *triggerMode_;
	/// Combo box holding the image mode.
	QComboBox *imageMode_;

	// CCD setup things.
	/// CCD file path line edit.
	QLineEdit *filePathEdit_;
	/// CCD file name line edit.
	QLineEdit *fileNameEdit_;
	/// CCD file number line edit.
	QLineEdit *fileNumberEdit_;
};

#endif // VESPERSROPERCCDDETECTORVIEW_H
