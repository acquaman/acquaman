#ifndef ConnectionSettings_H
#define ConnectionSettings_H

#include <QtGui>
#include "ui_ConnectionSettings.h"

#include "beamline/Beamline.h"
#include "beamline/SGMBeamline.h"
#include "beamline/ControlState.h"
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
		
//                qDebug() << "Energy value is " << SGMBeamline::sgm()->energy()->value() << " and slit value is " << SGMBeamline::sgm()->exitSlitGap()->value();
                ControlState *csTest = new ControlState(SGMBeamline::sgm(), this);
                csTest->vomit();

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

                /*
		connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(valueChanged(double)), hxpd_x_read, SLOT(setValue(double)));
		// TODO: errors. connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(pvError(int)), this, SLOT(onEpicsError(int)));
		connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(connected(bool)), hxpd_x_read, SLOT(setEnabled(bool)));
		connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(unitsChanged(const QString&)), hxpd_units, SLOT(setText(const QString&)));
		connect(Beamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(moving(bool)), hxpd_moving, SLOT(setChecked(bool)));
		connect(this->hxpd_move, SIGNAL(clicked()), this, SLOT(onMoveSent()));
                */

                connect(SGMBeamline::sgm()->energy(), SIGNAL(valueChanged(double)), hxpd_x_read, SLOT(setValue(double)));
                connect(SGMBeamline::sgm()->energy(), SIGNAL(connected(bool)), hxpd_x_read, SLOT(setEnabled(bool)));
                connect(SGMBeamline::sgm()->energy(), SIGNAL(unitsChanged(const QString&)), hxpd_units, SLOT(setText(const QString&)));
                connect(SGMBeamline::sgm()->energy(), SIGNAL(moving(bool)), hxpd_moving, SLOT(setChecked(bool)));
                connect(this->hxpd_move, SIGNAL(clicked()), this, SLOT(onMoveSent()));

                connect(this->hxpd_save, SIGNAL(clicked()), this, SLOT(onSaveRequested()));
                connect(this->hxpd_restore, SIGNAL(clicked()), this, SLOT(onRestoreRequested()));
                connect(this->energy_restore, SIGNAL(clicked()), this, SLOT(onEnergyRestoreRequested()));
                connect(this->setlist, SIGNAL(clicked()), this, SLOT(onSetListRequested()));

                connect(SGMBeamline::sgm()->m4(), SIGNAL(valueChanged(double)), m4_read, SLOT(setValue(double)));
                connect(SGMBeamline::sgm()->m4(), SIGNAL(connected(bool)), m4_read, SLOT(setEnabled(bool)));
                connect(SGMBeamline::sgm()->m4(), SIGNAL(unitsChanged(QString)), m4_units, SLOT(setText(QString)));
                connect(SGMBeamline::sgm()->m4(), SIGNAL(moving(bool)), m4_moving, SLOT(setChecked(bool)));

                connect(SGMBeamline::sgm()->m4()->child(0), SIGNAL(valueChanged(double)), m4inboard_read, SLOT(setValue(double)));
                connect(SGMBeamline::sgm()->m4()->child(0), SIGNAL(connected(bool)), m4inboard_read, SLOT(setEnabled(bool)));
                connect(SGMBeamline::sgm()->m4()->child(0), SIGNAL(unitsChanged(QString)), m4inboard_units, SLOT(setText(QString)));
                connect(SGMBeamline::sgm()->m4()->child(0), SIGNAL(moving(bool)), m4inboard_moving, SLOT(setChecked(bool)));
                connect(this->m4inboard_move, SIGNAL(clicked()), this, SLOT(onM4InboardMoveSent()));

                connect(SGMBeamline::sgm()->m4()->child(1), SIGNAL(valueChanged(double)), m4outboard_read, SLOT(setValue(double)));
                connect(SGMBeamline::sgm()->m4()->child(1), SIGNAL(connected(bool)), m4outboard_read, SLOT(setEnabled(bool)));
                connect(SGMBeamline::sgm()->m4()->child(1), SIGNAL(unitsChanged(QString)), m4outboard_units, SLOT(setText(QString)));
                connect(SGMBeamline::sgm()->m4()->child(1), SIGNAL(moving(bool)), m4outboard_moving, SLOT(setChecked(bool)));
                connect(this->m4outboard_move, SIGNAL(clicked()), this, SLOT(onM4OutboardMoveSent()));

                connect(SGMBeamline::sgm()->m4()->child(2), SIGNAL(valueChanged(double)), m4downstream_read, SLOT(setValue(double)));
                connect(SGMBeamline::sgm()->m4()->child(2), SIGNAL(connected(bool)), m4downstream_read, SLOT(setEnabled(bool)));
                connect(SGMBeamline::sgm()->m4()->child(2), SIGNAL(unitsChanged(QString)), m4downstream_units, SLOT(setText(QString)));
                connect(SGMBeamline::sgm()->m4()->child(2), SIGNAL(moving(bool)), m4downstream_moving, SLOT(setChecked(bool)));
                connect(this->m4downstream_move, SIGNAL(clicked()), this, SLOT(onM4DownstreamMoveSent()));

		NumericControl* nc = new NumericControl(Beamline::bl()->spectrometer()->hexapod()->x(), placeHolder);
                Q_UNUSED(nc);
                NumericControl *nc2 = new NumericControl(SGMBeamline::sgm()->grating(), gratingHolder);
                Q_UNUSED(nc2);

                csTest = NULL;

				// more crap in Connection settings. TODOTODOTOD get outta here!
				QSystemTrayIcon* sicon = new QSystemTrayIcon();
				sicon->show();
				sicon->showMessage("Fuck off, Dave!", "Isn't this cool?", QSystemTrayIcon::Information, 5000);

	}
	
public slots:
	void onEpicsError(int errCode) {
		
		textBrowser->append( DoubleProcessVariable::errorString(errCode));
		
	}

	void onMoveSent() {
//		Beamline::bl()->spectrometer()->hexapod()->x()->move(hxpd_x_set->value());
            SGMBeamline::sgm()->energy()->move(hxpd_x_set->value());
	}

        void onM4InboardMoveSent(){
            SGMBeamline::sgm()->m4()->child(0)->move(m4inboard_set->value());
        }

        void onM4OutboardMoveSent(){
            SGMBeamline::sgm()->m4()->child(1)->move(m4outboard_set->value());
        }

        void onM4DownstreamMoveSent(){
            SGMBeamline::sgm()->m4()->child(2)->move(m4downstream_set->value());
        }

        void onSaveRequested(){
            if(csTest)
                delete [] csTest;
            csTest = new ControlState(SGMBeamline::sgm(), this);
            csTest->vomit();
        }

        void onRestoreRequested(){
            if(csTest)
                csTest->restore(SGMBeamline::sgm());
        }

        void onEnergyRestoreRequested(){
           if(csTest){
                QList<QString> energyList;
                energyList << "energy";
                csTest->restoreList(SGMBeamline::sgm(), &energyList);
            }
        }

        void onSetListRequested(){
            QMap<QString, double> myList;
            myList["energy"] = 227.27;
            myList["M4Outboard"] = 12.12;
            myList["yourFace"] = 666;
            myList["mono"] = 2020;
            myList["exitSlitGap"] = 1985;
            int errLevel = this->setlist_errorLevel->value();
            if(SGMBeamline::sgm()->setStateList(myList, errLevel))
                qDebug() << "Success with error level of " << errLevel;
            else
                qDebug() << "Failure with error level of " << errLevel;
        }
	
protected:
    ControlState *csTest;
};

#endif
