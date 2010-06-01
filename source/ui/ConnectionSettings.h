#ifndef ACQMAN_ConnectionSettings_H
#define ACQMAN_ConnectionSettings_H

#include <QtGui>
#include "ui_ConnectionSettings.h"

#include "beamline/AMBeamline.h"
#include "beamline/SGMBeamline.h"
#include "beamline/AMControlState.h"
#include "ui/NumericControl.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"
#include "dataman/SGMLegacyFileImporter.h"

#include "../MPlot/src/MPlot/MPlotWidget.h"
#include "../MPlot/src/MPlot/MPlotSeriesData.h"
#include "../MPlot/src/MPlot/MPlotSeries.h"
#include "../MPlot/src/MPlot/MPlotImageData.h"
#include "../MPlot/src/MPlot/MPlotImage.h"
#include "../MPlot/src/MPlot/MPlotTools.h"


#include "ui/AMScanView.h"

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
				NumericControl *nc12 = new NumericControl(SGMBeamline::sgm()->harmonic(), harmonicHolder);
				Q_UNUSED(nc12);

				slitGapDoubleSpinBox->setMaximum(250);
				slitGapDoubleSpinBox->setMinimum(5);



				undulatorTrackingSpinBox->setMaximum(1);
				undulatorTrackingSpinBox->setMinimum(0);


				monoTrackingSpinBox->setMaximum(1);
				monoTrackingSpinBox->setMinimum(0);

				exitSlitTrackingSpinBox->setMaximum(1);
				exitSlitTrackingSpinBox->setMinimum(0);

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

			AMXASScan *s1 = new AMXASScan();
			SGMLegacyFileImporter *s1Loader = new SGMLegacyFileImporter(s1);
			if(!s1Loader->loadFromFile(AMUserSettings::userDataFolder + "001.dat"))
					qDebug() << "FAIL WHALE!";



			AMScanView* sv = new AMScanView();
			sv->show();
			sv->changeViewMode(AMScanView::MultiScans);
			sv->addScan(s1);
			sv->changeViewMode(AMScanView::MultiChannels);
			sv->addScan(s1);


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
