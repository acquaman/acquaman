#ifndef ACQMAN_ConnectionSettings_H
#define ACQMAN_ConnectionSettings_H

#include <QtGui>
#include "ui_ConnectionSettings.h"

#include "beamline/AMBeamline.h"
#include "beamline/AMSGMBeamline.h"
#include "beamline/AMControlState.h"
#include "ui/NumericControl.h"
#include "acquaman/AMXASDacqScanController.h"

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
		
//                qDebug() << "Energy value is " << AMSGMBeamline::sgm()->energy()->value() << " and slit value is " << AMSGMBeamline::sgm()->exitSlitGap()->value();
                AMControlState *csTest = new AMControlState(AMSGMBeamline::sgm(), this);
                csTest->vomit();

                connect(AMBeamline::bl()->ringCurrent(), SIGNAL(valueChanged(double)), doubleSpinBox, SLOT(setValue(double)));
                // TODO: connect(AMBeamline::bl()->ringCurrent(), SIGNAL(pvError(int)), this, SLOT(onEpicsError(int)));
                connect(AMBeamline::bl()->ringCurrent(), SIGNAL(connected(bool)), doubleSpinBox, SLOT(setEnabled(bool)));
                connect(AMBeamline::bl()->ringCurrent(), SIGNAL(unitsChanged(const QString&)), unitLabel, SLOT(setText(const QString&)));

		/*
		StringProcessVariable* ringI3 = new StringProcessVariable("PCT1402-01:mA:fbk", 1, this);
		connect(ringI3, SIGNAL(valueChanged(const QString&)), lineEdit, SLOT(setText(const QString&)));
		connect(ringI3, SIGNAL(error(int)), this, SLOT(onEpicsError(int)));
		connect(ringI3, SIGNAL(connected(bool)), lineEdit, SLOT(setEnabled(bool)));
		*/

                /*
                connect(AMBeamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(valueChanged(double)), hxpd_x_read, SLOT(setValue(double)));
                // TODO: errors. connect(AMBeamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(pvError(int)), this, SLOT(onEpicsError(int)));
                connect(AMBeamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(connected(bool)), hxpd_x_read, SLOT(setEnabled(bool)));
                connect(AMBeamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(unitsChanged(const QString&)), hxpd_units, SLOT(setText(const QString&)));
                connect(AMBeamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(moving(bool)), hxpd_moving, SLOT(setChecked(bool)));
		connect(this->hxpd_move, SIGNAL(clicked()), this, SLOT(onMoveSent()));
                */

                connect(this->hxpd_save, SIGNAL(clicked()), this, SLOT(onSaveRequested()));
                connect(this->hxpd_restore, SIGNAL(clicked()), this, SLOT(onRestoreRequested()));
                connect(this->energy_restore, SIGNAL(clicked()), this, SLOT(onEnergyRestoreRequested()));
                connect(this->setlist, SIGNAL(clicked()), this, SLOT(onSetListRequested()));
                /*
                connect(AMSGMBeamline::sgm()->energy(), SIGNAL(valueChanged(double)), hxpd_x_read, SLOT(setValue(double)));
                connect(AMSGMBeamline::sgm()->energy(), SIGNAL(connected(bool)), hxpd_x_read, SLOT(setEnabled(bool)));
                connect(AMSGMBeamline::sgm()->energy(), SIGNAL(unitsChanged(const QString&)), hxpd_units, SLOT(setText(const QString&)));
                connect(AMSGMBeamline::sgm()->energy(), SIGNAL(moving(bool)), hxpd_moving, SLOT(setChecked(bool)));
                connect(this->hxpd_move, SIGNAL(clicked()), this, SLOT(onMoveSent()));

                connect(AMSGMBeamline::sgm()->m4(), SIGNAL(valueChanged(double)), m4_read, SLOT(setValue(double)));
                connect(AMSGMBeamline::sgm()->m4(), SIGNAL(connected(bool)), m4_read, SLOT(setEnabled(bool)));
                connect(AMSGMBeamline::sgm()->m4(), SIGNAL(unitsChanged(QString)), m4_units, SLOT(setText(QString)));
                connect(AMSGMBeamline::sgm()->m4(), SIGNAL(moving(bool)), m4_moving, SLOT(setChecked(bool)));

                connect(AMSGMBeamline::sgm()->m4()->child(0), SIGNAL(valueChanged(double)), m4inboard_read, SLOT(setValue(double)));
                connect(AMSGMBeamline::sgm()->m4()->child(0), SIGNAL(connected(bool)), m4inboard_read, SLOT(setEnabled(bool)));
                connect(AMSGMBeamline::sgm()->m4()->child(0), SIGNAL(unitsChanged(QString)), m4inboard_units, SLOT(setText(QString)));
                connect(AMSGMBeamline::sgm()->m4()->child(0), SIGNAL(moving(bool)), m4inboard_moving, SLOT(setChecked(bool)));
                connect(this->m4inboard_move, SIGNAL(clicked()), this, SLOT(onM4InboardMoveSent()));

                connect(AMSGMBeamline::sgm()->m4()->child(1), SIGNAL(valueChanged(double)), m4outboard_read, SLOT(setValue(double)));
                connect(AMSGMBeamline::sgm()->m4()->child(1), SIGNAL(connected(bool)), m4outboard_read, SLOT(setEnabled(bool)));
                connect(AMSGMBeamline::sgm()->m4()->child(1), SIGNAL(unitsChanged(QString)), m4outboard_units, SLOT(setText(QString)));
                connect(AMSGMBeamline::sgm()->m4()->child(1), SIGNAL(moving(bool)), m4outboard_moving, SLOT(setChecked(bool)));
                connect(this->m4outboard_move, SIGNAL(clicked()), this, SLOT(onM4OutboardMoveSent()));

                connect(AMSGMBeamline::sgm()->m4()->child(2), SIGNAL(valueChanged(double)), m4downstream_read, SLOT(setValue(double)));
                connect(AMSGMBeamline::sgm()->m4()->child(2), SIGNAL(connected(bool)), m4downstream_read, SLOT(setEnabled(bool)));
                connect(AMSGMBeamline::sgm()->m4()->child(2), SIGNAL(unitsChanged(QString)), m4downstream_units, SLOT(setText(QString)));
                connect(AMSGMBeamline::sgm()->m4()->child(2), SIGNAL(moving(bool)), m4downstream_moving, SLOT(setChecked(bool)));
                connect(this->m4downstream_move, SIGNAL(clicked()), this, SLOT(onM4DownstreamMoveSent()));
                */

                NumericControl* nc = new NumericControl(AMBeamline::bl()->spectrometer()->hexapod()->x(), placeHolder);
                Q_UNUSED(nc);
                NumericControl *nc2 = new NumericControl(AMSGMBeamline::sgm()->grating(), gratingHolder);
                Q_UNUSED(nc2);
                NumericControl *nc3 = new NumericControl(AMSGMBeamline::sgm()->energy(), energyHolder);
                Q_UNUSED(nc3);
                NumericControl *nc4 = new NumericControl(AMSGMBeamline::sgm()->exitSlitGap(), slitGapHolder);
                Q_UNUSED(nc4);
                NumericControl *nc5 = new NumericControl(AMSGMBeamline::sgm()->m4(), m4Holder);
                Q_UNUSED(nc5);
                NumericControl *nc6 = new NumericControl(AMSGMBeamline::sgm()->m4()->child(0), m4InboardHolder);
                Q_UNUSED(nc6);
                NumericControl *nc7 = new NumericControl(AMSGMBeamline::sgm()->m4()->child(1), m4OutboardHolder);
                Q_UNUSED(nc7);
                NumericControl *nc8 = new NumericControl(AMSGMBeamline::sgm()->m4()->child(2), m4DownstreamHolder);
                Q_UNUSED(nc8);

                csTest = NULL;



	}
	
public slots:
	void onEpicsError(int errCode) {
		
                textBrowser->append( AMDoubleProcessVariable::errorString(errCode));
		
	}

	void onMoveSent() {
//		AMBeamline::bl()->spectrometer()->hexapod()->x()->move(hxpd_x_set->value());
            //AMSGMBeamline::sgm()->energy()->move(hxpd_x_set->value());
	}

        void onM4InboardMoveSent(){
            //AMSGMBeamline::sgm()->m4()->child(0)->move(m4inboard_set->value());
        }

        void onM4OutboardMoveSent(){
            //AMSGMBeamline::sgm()->m4()->child(1)->move(m4outboard_set->value());
        }

        void onM4DownstreamMoveSent(){
            //AMSGMBeamline::sgm()->m4()->child(2)->move(m4downstream_set->value());
        }

        void onSaveRequested(){
            csTest = new AMControlState(AMSGMBeamline::sgm(), this);
            csTest->vomit();
            AMXASScanConfiguration *xasSC = new AMXASScanConfiguration(this);
        }

        void onRestoreRequested(){
            AMXASScanConfiguration *xasCfg = new AMXASScanConfiguration(AMSGMBeamline::sgm());
            xasCfg->addRegion(0, 250, 0.1, 251);
            xasCfg->setExitSlitGap(15);
            xasCfg->setGrating(AMSGMBeamline::mediumGrating);
            AMXASDacqScanController *xasCtrl = new AMXASDacqScanController(xasCfg, AMSGMBeamline::sgm());
            xasCtrl->initialize();
            if(csTest)
                csTest->restore(AMSGMBeamline::sgm());
        }

        void onEnergyRestoreRequested(){
           if(csTest){
                QList<QString> energyList;
                energyList << "energy";
                csTest->restoreList(AMSGMBeamline::sgm(), &energyList);
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
            if(AMSGMBeamline::sgm()->setStateList(myList, errLevel))
                qDebug() << "Success with error level of " << errLevel;
            else
                qDebug() << "Failure with error level of " << errLevel;
        }
	
protected:
    AMControlState *csTest;
};

#endif
