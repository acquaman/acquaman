#ifndef ConnectionSettings_H
#define ConnectionSettings_H

#include <QtGui>
#include "ui_ConnectionSettings.h"

#include "beamline/Beamline.h"
#include "ui/NumericControl.h"

class ConnectionSettings : public QWidget, private Ui::ConnectionSettings {
	
	Q_OBJECT
	
public:
	ConnectionSettings(QWidget* parent = 0) : QWidget(parent) {
		
		setupUi(this);
		spinBox->setEnabled(false);
		doubleSpinBox->setEnabled(false);
		lineEdit->setEnabled(false);
		unitLabel->setText("unset");
		
		/*
		IntProcessVariable* ringI1 = new IntProcessVariable("PCT1402-01:mA:fbk", 1, this);
		connect(ringI1, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
		connect(ringI1, SIGNAL(error(int)), this, SLOT(onEpicsError(int)));
		connect(ringI1, SIGNAL(connected(bool)), spinBox, SLOT(setEnabled(bool)));
		*/
		
		
		connect(Beamline::bl()->ringCurrent(), SIGNAL(valueChanged(double)), doubleSpinBox, SLOT(setValue(double)));
		// TODO: connect(Beamline::bl()->ringCurrent(), SIGNAL(pvError(int)), this, SLOT(onEpicsError(int)));
		connect(Beamline::bl()->ringCurrent(), SIGNAL(connected(bool)), doubleSpinBox, SLOT(setEnabled(bool)));
		connect(Beamline::bl()->ringCurrent(), SIGNAL(unitsChanged(const QString&)), unitLabel, SLOT(setText(const QString&)));
		
		/*
		StringProcessVariable* ringI3 = new StringProcessVariable("PCT1402-01:mA:fbk", 1, this);
		connect(ringI3, SIGNAL(valueChanged(const QString&)), lineEdit, SLOT(setText(const QString&)));
		connect(ringI3, SIGNAL(error(int)), this, SLOT(onEpicsError(int)));
		connect(ringI3, SIGNAL(connected(bool)), lineEdit, SLOT(setEnabled(bool)));
		*/

		connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(valueChanged(double)), hxpd_x_read, SLOT(setValue(double)));
		// TODO: errors. connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(pvError(int)), this, SLOT(onEpicsError(int)));
		connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(connected(bool)), hxpd_x_read, SLOT(setEnabled(bool)));
		connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(unitsChanged(const QString&)), hxpd_units, SLOT(setText(const QString&)));
		connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(moving(bool)), hxpd_moving, SLOT(setChecked(bool)));
		connect(this->hxpd_move, SIGNAL(clicked()), this, SLOT(onMoveSent()));

		NumericControl* nc = new NumericControl(Beamline::bl()->spectrometer()->hexapod()->x(), placeHolder);
                Q_UNUSED(nc);

	}
	
public slots:
	void onEpicsError(int errCode) {
		
		textBrowser->append( DoubleProcessVariable::errorString(errCode));
		
	}

	void onMoveSent() {
		Beamline::bl()->spectrometer()->hexapod()->x()->move(hxpd_x_set->value());
	}
	
};

#endif
