#ifndef CLSSYNCHRONIZEDDWELLTIMEVIEW_H
#define CLSSYNCHRONIZEDDWELLTIMEVIEW_H

#include <QWidget>

#include "beamline/CLS/CLSSynchronizedDwellTime.h"

#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>

/*!
  This widget takes in a single CLSSynchronizedDwellTimeElement and builds a view for it.  It has its own view because the elements are self-contained objects themselves.
  */
class CLSSynchronizedDwellTimeElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in a CLSSynchronizedDwellTimeElement.
	explicit CLSSynchronizedDwellTimeElementView(CLSSynchronizedDwellTimeElement *dwellTimeElement, QWidget *parent = 0);

protected slots:
	/// Sets the checked state.
	void setEnabledState(bool enable) { if (element_->isEnabled() != enable) element_->setEnabled(enable); }
	/// Sets the time.
	void setTime();

	/// Handles changes to the status.
	void onStatusChanged(bool active) { status_->setPixmap(QIcon((active == true) ? ":/ON.png" : ":/OFF.png").pixmap(25, 25)); }
	/// Handles setting the units for the suffix for the time once it is accessible.  Disconnects itself once an acceptable value has been set.
	void checkTimeUnits();
	/// Handles changes to the element enabled signal.
	void onEnabledChanged(bool enabled)
	{
		if (enabled){

			enable_->setText("Enabled");
			enable_->setPalette(QPalette(Qt::green));
		}
		else{

			enable_->setText("Disabled");
			enable_->setPalette(QPalette(Qt::red));
		}
	}

protected:
	/// The status icon label.
	QLabel *status_;
	/// The enabling button.
	QPushButton *enable_;
	/// The time spin box.
	QDoubleSpinBox *time_;

	/// Pointer to the element.
	CLSSynchronizedDwellTimeElement *element_;
};

/*!
  This widget takes in a CLSSynchronizedDwellTime object and builds a view for it.  The current look is based on the current EDM version found on VESPERS.
  */
class CLSSynchronizedDwellTimeView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes a CLSSynchronizedDwellTime object.
	explicit CLSSynchronizedDwellTimeView(CLSSynchronizedDwellTime *dwellTime, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Sets the time.
	void setTime();
	/// Sets the mode for the dwell time.  If the mode is true, then set the Mode to Continuous, otherwise set it to Single Shot.
	void setMode(bool mode);

	/// Handles changes to the status.
	void onStatusChanged(bool active) { overallStatus_->setPixmap(QIcon((active == true) ? ":/ON.png" : ":/OFF.png").pixmap(30, 30)); }
	/// Handles changes to the mode.
	void onModeChanged(CLSSynchronizedDwellTime::Mode mode)
	{
		if (mode == CLSSynchronizedDwellTime::SingleShot){

			mode_->setText("Single Shot");
			mode_->setChecked(false);
		}
		else if (mode == CLSSynchronizedDwellTime::Continuous){

			mode_->setText("Continuous");
			mode_->setChecked(true);
		}
	}

protected:
	/// The status icon label.
	QLabel *overallStatus_;
	/// The mode selection button.
	QPushButton *mode_;
	/// The master time double spin box.
	QDoubleSpinBox *masterTime_;

	/// Pointer to the synchronized dwell time object.
	CLSSynchronizedDwellTime *dwellTime_;
};

#endif // CLSSYNCHRONIZEDDWELLTIMEVIEW_H
