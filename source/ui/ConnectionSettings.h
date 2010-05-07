#ifndef ACQMAN_ConnectionSettings_H
#define ACQMAN_ConnectionSettings_H

#include <QtGui>
#include "ui_ConnectionSettings.h"

#include "beamline/AMBeamline.h"
#include "beamline/SGMBeamline.h"
#include "beamline/AMControlState.h"
#include "ui/NumericControl.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"

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
				AMControlState *csTest = new AMControlState(SGMBeamline::sgm(), this);
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
				connect(AMBeamline::bl()->spectrometer()->hexapod()->x(), SIGNAL(movingChanged(bool)), hxpd_moving, SLOT(setChecked(bool)));
		connect(this->hxpd_move, SIGNAL(clicked()), this, SLOT(onMoveSent()));
				*/

				connect(this->hxpd_save, SIGNAL(clicked()), this, SLOT(onSaveRequested()));
				connect(this->hxpd_restore, SIGNAL(clicked()), this, SLOT(onRestoreRequested()));
				connect(this->energy_restore, SIGNAL(clicked()), this, SLOT(onEnergyRestoreRequested()));
				connect(this->setlist, SIGNAL(clicked()), this, SLOT(onSetListRequested()));
				connect(this->setupScanButton, SIGNAL(clicked()), this, SLOT(onSetupScan()));
				connect(this->scanButton, SIGNAL(clicked()), this, SLOT(onStartScan()));
				/*
				connect(SGMBeamline::sgm()->energy(), SIGNAL(valueChanged(double)), hxpd_x_read, SLOT(setValue(double)));
				connect(SGMBeamline::sgm()->energy(), SIGNAL(connected(bool)), hxpd_x_read, SLOT(setEnabled(bool)));
				connect(SGMBeamline::sgm()->energy(), SIGNAL(unitsChanged(const QString&)), hxpd_units, SLOT(setText(const QString&)));
								connect(SGMBeamline::sgm()->energy(), SIGNAL(movingChanged(bool)), hxpd_moving, SLOT(setChecked(bool)));
				connect(this->hxpd_move, SIGNAL(clicked()), this, SLOT(onMoveSent()));

				connect(SGMBeamline::sgm()->m4(), SIGNAL(valueChanged(double)), m4_read, SLOT(setValue(double)));
				connect(SGMBeamline::sgm()->m4(), SIGNAL(connected(bool)), m4_read, SLOT(setEnabled(bool)));
				connect(SGMBeamline::sgm()->m4(), SIGNAL(unitsChanged(QString)), m4_units, SLOT(setText(QString)));
								connect(SGMBeamline::sgm()->m4(), SIGNAL(movingChanged(bool)), m4_moving, SLOT(setChecked(bool)));

				connect(SGMBeamline::sgm()->m4()->child(0), SIGNAL(valueChanged(double)), m4inboard_read, SLOT(setValue(double)));
				connect(SGMBeamline::sgm()->m4()->child(0), SIGNAL(connected(bool)), m4inboard_read, SLOT(setEnabled(bool)));
				connect(SGMBeamline::sgm()->m4()->child(0), SIGNAL(unitsChanged(QString)), m4inboard_units, SLOT(setText(QString)));
								connect(SGMBeamline::sgm()->m4()->child(0), SIGNAL(movingChanged(bool)), m4inboard_moving, SLOT(setChecked(bool)));
				connect(this->m4inboard_move, SIGNAL(clicked()), this, SLOT(onM4InboardMoveSent()));

				connect(SGMBeamline::sgm()->m4()->child(1), SIGNAL(valueChanged(double)), m4outboard_read, SLOT(setValue(double)));
				connect(SGMBeamline::sgm()->m4()->child(1), SIGNAL(connected(bool)), m4outboard_read, SLOT(setEnabled(bool)));
				connect(SGMBeamline::sgm()->m4()->child(1), SIGNAL(unitsChanged(QString)), m4outboard_units, SLOT(setText(QString)));
								connect(SGMBeamline::sgm()->m4()->child(1), SIGNAL(movingChanged(bool)), m4outboard_moving, SLOT(setChecked(bool)));
				connect(this->m4outboard_move, SIGNAL(clicked()), this, SLOT(onM4OutboardMoveSent()));

				connect(SGMBeamline::sgm()->m4()->child(2), SIGNAL(valueChanged(double)), m4downstream_read, SLOT(setValue(double)));
				connect(SGMBeamline::sgm()->m4()->child(2), SIGNAL(connected(bool)), m4downstream_read, SLOT(setEnabled(bool)));
				connect(SGMBeamline::sgm()->m4()->child(2), SIGNAL(unitsChanged(QString)), m4downstream_units, SLOT(setText(QString)));
								connect(SGMBeamline::sgm()->m4()->child(2), SIGNAL(movingChanged(bool)), m4downstream_moving, SLOT(setChecked(bool)));
				connect(this->m4downstream_move, SIGNAL(clicked()), this, SLOT(onM4DownstreamMoveSent()));
				*/

				NumericControl* nc = new NumericControl(AMBeamline::bl()->spectrometer()->hexapod()->x(), placeHolder);
				Q_UNUSED(nc);
				NumericControl *nc2 = new NumericControl(SGMBeamline::sgm()->grating(), gratingHolder);
				Q_UNUSED(nc2);
				NumericControl *nc3 = new NumericControl(SGMBeamline::sgm()->energy(), energyHolder);
				Q_UNUSED(nc3);
				NumericControl *nc4 = new NumericControl(SGMBeamline::sgm()->exitSlitGap(), slitGapHolder);
				Q_UNUSED(nc4);
				NumericControl *nc5 = new NumericControl(SGMBeamline::sgm()->m4(), m4Holder);
				Q_UNUSED(nc5);
				NumericControl *nc6 = new NumericControl(SGMBeamline::sgm()->m4()->child(0), m4InboardHolder);
				Q_UNUSED(nc6);
				NumericControl *nc7 = new NumericControl(SGMBeamline::sgm()->m4()->child(1), m4OutboardHolder);
				Q_UNUSED(nc7);
				NumericControl *nc8 = new NumericControl(SGMBeamline::sgm()->m4()->child(2), m4DownstreamHolder);
				Q_UNUSED(nc8);
				NumericControl *nc9 = new NumericControl(SGMBeamline::sgm()->undulatorTracking(), uTrackHolder);
				Q_UNUSED(nc9);
				NumericControl *nc10 = new NumericControl(SGMBeamline::sgm()->monoTracking(), mTrackHolder);
				Q_UNUSED(nc10);
				NumericControl *nc11 = new NumericControl(SGMBeamline::sgm()->exitSlitTracking(), eTrackHolder);
				Q_UNUSED(nc11);

//                slitGapDoubleSpinBox->setMaximum(SGMBeamline::sgm()->exitSlitGap()->maximumValue());
//                slitGapDoubleSpinBox->setMinimum(SGMBeamline::sgm()->exitSlitGap()->minimumValue());
				slitGapDoubleSpinBox->setMaximum(250);
				slitGapDoubleSpinBox->setMinimum(5);



 //               undulatorTrackingSpinBox->setMaximum(SGMBeamline::sgm()->undulatorTracking()->maximumValue());
 //               undulatorTrackingSpinBox->setMinimum(SGMBeamline::sgm()->undulatorTracking()->minimumValue());
				undulatorTrackingSpinBox->setMaximum(1);
				undulatorTrackingSpinBox->setMinimum(0);


//                monoTrackingSpinBox->setMaximum(SGMBeamline::sgm()->monoTracking()->maximumValue());
//                monoTrackingSpinBox->setMinimum(SGMBeamline::sgm()->monoTracking()->minimumValue());
				monoTrackingSpinBox->setMaximum(1);
				monoTrackingSpinBox->setMinimum(0);

//                exitSlitTrackingSpinBox->setMaximum(SGMBeamline::sgm()->exitSlitTracking()->maximumValue());
//                exitSlitTrackingSpinBox->setMinimum(SGMBeamline::sgm()->exitSlitTracking()->minimumValue());
				exitSlitTrackingSpinBox->setMaximum(1);
				exitSlitTrackingSpinBox->setMinimum(0);

//                gratingSpinBox->setMaximum(SGMBeamline::sgm()->grating()->maximumValue());
//                gratingSpinBox->setMinimum(SGMBeamline::sgm()->grating()->minimumValue());
				gratingSpinBox->setMaximum(2);
				gratingSpinBox->setMinimum(0);

				csTest = NULL;
				cfg = NULL;
				ctrl = NULL;
	}

public slots:
	void onEpicsError(int errCode) {

				textBrowser->append( AMProcessVariable::errorString(errCode));

	}

	void onMoveSent() {
//		AMBeamline::bl()->spectrometer()->hexapod()->x()->move(hxpd_x_set->value());
			//SGMBeamline::sgm()->energy()->move(hxpd_x_set->value());
	}

		void onM4InboardMoveSent(){
			//SGMBeamline::sgm()->m4()->child(0)->move(m4inboard_set->value());
		}

		void onM4OutboardMoveSent(){
			//SGMBeamline::sgm()->m4()->child(1)->move(m4outboard_set->value());
		}

		void onM4DownstreamMoveSent(){
			//SGMBeamline::sgm()->m4()->child(2)->move(m4downstream_set->value());
		}

		void onSaveRequested(){
			csTest = new AMControlState(SGMBeamline::sgm(), this);
			csTest->vomit();
		}

		void onRestoreRequested(){

			/*
			qDebug() << "Doing scan?";

			SGMXASScanConfiguration *xasCfg = new SGMXASScanConfiguration(SGMBeamline::sgm());
			xasCfg->addRegion(0, 250, 1, 260);
			xasCfg->setExitSlitGap(15);
			xasCfg->setGrating(SGMBeamline::mediumGrating);
			SGMXASDacqScanController *xasCtrl = new SGMXASDacqScanController(xasCfg, SGMBeamline::sgm());

			xasCtrl->initialize();
			xasCtrl->start();
			*/


			/*
			SGMFluxOptimization firstTry;
			QList<QVariant> flux1, flux2, flux3, flux4;
			flux1 << 250.0 << 0 << 1;
			flux2 << 250.0 << 1 << 1;
			flux3 << 250.0 << 2 << 1;
			flux4 << 250.0 << 2 << 3;
			qDebug() << " BEFORE EVERYTHING " << xasCfg->regions().at(0)->start() << " " << xasCfg->regions().at(0)->delta() << " " << xasCfg->regions().at(0)->end();
			QMap<double, double> map1 = firstTry.curve(flux1, xasCfg->regions());
			QMap<double, double> map2 = firstTry.curve(flux2, xasCfg->regions());
			QMap<double, double> map3 = firstTry.curve(flux3, xasCfg->regions());
			QMap<double, double> map4 = firstTry.curve(flux4, xasCfg->regions());
			flux1[0] = 100.0;
			flux2[0] = 100.0;
			flux3[0] = 100.0;
			flux4[0] = 100.0;
			QMap<double, double> map5 = firstTry.curve(flux1, xasCfg->regions());
			QMap<double, double> map6 = firstTry.curve(flux2, xasCfg->regions());
			QMap<double, double> map7 = firstTry.curve(flux3, xasCfg->regions());
			QMap<double, double> map8 = firstTry.curve(flux4, xasCfg->regions());


			for(int x = 200; x < 2000; x++)
				qDebug() << (double)x
						<<  map1[(double)x]
						<< map2[(double)x]
						<< map3[(double)x]
						<< map4[(double)x]
						<< map5[(double)x]
						<< map6[(double)x]
						<< map7[(double)x]
						<< map8[(double)x];
			*/

/*            QMap<double, double>::const_iterator i = firstMap.constBegin();
			while (i != firstMap.constEnd()) {
				qDebug() << i.key() << " " << i.value();
				++i;
			}
*/

//			if(csTest)
//				csTest->restore(SGMBeamline::sgm());
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
						if(SGMBeamline::sgm()->setState(myList, errLevel))
				qDebug() << "Success with error level of " << errLevel;
			else
				qDebug() << "Failure with error level of " << errLevel;
		}

		void onSetupScan(){
			if(!SGMBeamline::sgm()->isConnected())
				return;
			if(!cfg){
				cfg = new SGMXASScanConfiguration(SGMBeamline::sgm());
				((SGMXASScanConfiguration*)cfg)->addRegion(0, 250, 1, 370);

				slitGapDoubleSpinBox->setValue(((SGMXASScanConfiguration*)cfg)->exitSlitGap());
				connect(slitGapDoubleSpinBox, SIGNAL(valueChanged(double)), cfg, SLOT(setExitSlitGap(double)));
				connect(cfg, SIGNAL(exitSlitGapChanged(double)), slitGapDoubleSpinBox, SLOT(setValue(double)));

				undulatorTrackingSpinBox->setValue(((SGMXASScanConfiguration*)cfg)->undulatorTracking());
				connect(undulatorTrackingSpinBox, SIGNAL(valueChanged(int)), cfg, SLOT(setUndulatorTracking(int)));
				connect(cfg, SIGNAL(undulatorTrackingChanged(int)), undulatorTrackingSpinBox, SLOT(setValue(int)));

				monoTrackingSpinBox->setValue(((SGMXASScanConfiguration*)cfg)->monoTracking());
				connect(monoTrackingSpinBox, SIGNAL(valueChanged(int)), cfg, SLOT(setUndulatorTracking(int)));
				connect(cfg, SIGNAL(monoTrackingChanged(int)), monoTrackingSpinBox, SLOT(setValue(int)));

				exitSlitTrackingSpinBox->setValue(((SGMXASScanConfiguration*)cfg)->exitSlitTracking());
				connect(exitSlitTrackingSpinBox, SIGNAL(valueChanged(int)), cfg, SLOT(setUndulatorTracking(int)));
				connect(cfg, SIGNAL(exitSlitTrackingChanged(int)), exitSlitTrackingSpinBox, SLOT(setValue(int)));

				ctrl = new SGMXASDacqScanController(((SGMXASScanConfiguration*)cfg), SGMBeamline::sgm());
				connect(ctrl, SIGNAL(progress(double,double)), this, SIGNAL(scanProgress(double,double)));
				return;
			}
		}

		void onStartScan(){
			if(!SGMBeamline::sgm()->isConnected())
				return;
			if(!cfg)
				onSetupScan();
			ctrl->initialize();
			ctrl->start();
		}

		void onStopScan(){
			if(ctrl && ctrl->isRunning()){
				ctrl->cancel();
			}
		}

signals:
	void scanProgress(double elapsed, double total);

protected:
	AMControlState *csTest;
	AMScanConfiguration *cfg;
	AMScanController *ctrl;
//    SGMXASScanConfiguration *xasCfg;
//    SGMXASDacqScanController *xasCtrl;
};

#endif
