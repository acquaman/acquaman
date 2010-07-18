#ifndef AMCONTROLSETVIEW_H
#define AMCONTROLSETVIEW_H

#include "beamline/AMControlSet.h"
#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QAbstractSpinBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>

#include "../MPlot/src/MPlot/MPlotWidget.h"
#include "../MPlot/src/MPlot/MPlotSeriesData.h"
#include "../MPlot/src/MPlot/MPlotSeries.h"
#include "../MPlot/src/MPlot/MPlotImageData.h"
#include "../MPlot/src/MPlot/MPlotImage.h"
#include "../MPlot/src/MPlot/MPlotTools.h"

#include <QTableView>
#include <QPen>
#include <QBrush>

/// An AMControlSetView is a means of generating a default view for an AMControlSet
/*!
  Inheriting from QGroupBox, the AMControlSetView displays the contents of an AMControlSet in a group box.
  Simplying calling the constructor while passing an AMControlSet allows the AMControlSetView to:
  - set the title of the group box from the name of the AMControlSet; and,
  - generate a label for each AMControl in the AMControlSet from the objectName() function; and,
  - generate a spin box (integer for enums and double for all others) for each AMControl in the AMControlSet.
  */
class AMControlSetView : public QGroupBox
{
Q_OBJECT
public:
	/// Constructor
	/*! \param viewSet Pointer to the AMControlSet to view.
	  \param parent Pointer to QWidget to act as parent.
	  */
	explicit AMControlSetView(AMControlSet *viewSet, QWidget *parent = 0);

	QWidget* boxByName(const QString &name){
		return controlBoxes_.at(viewSet_->indexOf(name));
	}

	QWidget* boxAt(int row){
		return controlBoxes_.at(row);
	}

signals:

public slots:

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMControlSet *viewSet_;
	QList<QWidget*> controlBoxes_;
	QHBoxLayout *hl_;
};

class AMQuickDataSet : public QObject
{
	Q_OBJECT
public:
	AMQuickDataSet(QMap<double, double> dataMap, QObject *parent = 0) : QObject(parent){
		model_ = NULL;
		setDataMap(dataMap);
	}

	double minY() { return minY_; }
	double maxY() { return maxY_; }
	double minX() { return minX_; }
	double maxX() { return maxX_; }

	void setModel(MPlotRealtimeModel *model){ model_ = model; setupModel();}

	void setupModel(){
		while (model_->count() > 0) {
			model_->removePointBack();
		}
		QMap<double, double>::const_iterator i = dataMap_.constBegin();
		while (i != dataMap_.constEnd()) {
			model_->insertPointBack(i.key(), i.value());
			++i;
		}
	}

	void setDataMap(QMap<double, double> dataMap){
		dataMap_ = dataMap;
		QMap<double, double>::const_iterator i = dataMap_.constBegin();
		minX_ = i.key();
		minY_ = i.value();
		maxX_ = i.key();
		maxY_ = i.value();
		double tmpX, tmpY;
		qDebug() << tmpX << " " << tmpY;
		while(i != dataMap_.constEnd()){
			tmpX = i.key();
			tmpY = i.value();
			if(tmpX < minX_)
				minX_ = tmpX;
			else if(tmpX > maxX_)
				maxX_ = tmpX;
			if(tmpY < minY_)
				minY_ = tmpY;
			else if(tmpY > maxY_)
				maxY_ = tmpY;
			qDebug() << tmpX << " " << tmpY;
			++i;
		}
		if(model_)
			setupModel();
	}

protected:
	QMap<double, double> dataMap_;
	double minX_;
	double minY_;
	double maxX_;
	double maxY_;
	MPlotRealtimeModel *model_;
};

class AMControlOptimizationSetView : public AMControlSetView
{
	Q_OBJECT
public:
	AMControlOptimizationSetView(AMControlOptimizationSet *viewSet, QWidget *parent = 0);

public slots:
	void onRegionsUpdate(AMRegionsList* contextParams){

		/*
		QList<QVariant> l1, m1, h1, h3;
		l1 << 250.0 << 0 << 1;
		m1 << 250.0 << 1 << 1;
		h1 << 250.0 << 2 << 1;
		h3 << 250.0 << 2 << 3;

		int count = 0;
		double stepSize = 250/(numPoints-1);
		QMap<double, double> fl1, fm1, fh1, fh3, rl1, rm1, rh1, rh3;
		for(double x = stepSize; x < 250; x+=stepSize){
			l1.replace(0, x);
			m1.replace(0, x);
			h1.replace(0, x);
			h3.replace(0, x);
			fluxL1.at(count)->setDataMap( ((AMControlOptimizationSet*)viewSet_)->curveAt(0, l1, contextParams) );
			fluxM1.at(count)->setDataMap( ((AMControlOptimizationSet*)viewSet_)->curveAt(0, m1, contextParams) );
			fluxH1.at(count)->setDataMap( ((AMControlOptimizationSet*)viewSet_)->curveAt(0, h1, contextParams) );
			fluxH3.at(count)->setDataMap( ((AMControlOptimizationSet*)viewSet_)->curveAt(0, h3, contextParams) );
			resL1.at(count)->setDataMap( ((AMControlOptimizationSet*)viewSet_)->curveAt(1, l1, contextParams) );
			resM1.at(count)->setDataMap( ((AMControlOptimizationSet*)viewSet_)->curveAt(1, m1, contextParams) );
			resH1.at(count)->setDataMap( ((AMControlOptimizationSet*)viewSet_)->curveAt(1, h1, contextParams) );
			resH3.at(count)->setDataMap( ((AMControlOptimizationSet*)viewSet_)->curveAt(1, h3, contextParams) );
			fl1.insert(x, fluxL1.at(count)->minY());
			fm1.insert(x, fluxM1.at(count)->minY());
			fh1.insert(x, fluxH1.at(count)->minY());
			fh3.insert(x, fluxH3.at(count)->minY());
			rl1.insert(x, resL1.at(count)->minY());
			rm1.insert(x, resM1.at(count)->minY());
			rh1.insert(x, resH1.at(count)->minY());
			rh3.insert(x, resH3.at(count)->minY());
//			qDebug() << "Fluxes: L1 " << fluxL1.at(count)->minY() << " M1 " << fluxM1.at(count)->minY() << " H1 " << fluxH1.at(count)->minY()
//					<< " H3 " << fluxH3.at(count)->minY() << "\n"
//					<< "Resolutions: L1 " << resL1.at(count)->minY() << " M1 " << resM1.at(count)->minY() << " H1 " << resH1.at(count)->minY()
//					<< " H3 " << resH3.at(count)->minY() << "\n";
			count++;
		}
		aFluxL1->setDataMap(fl1);
		aFluxM1->setDataMap(fm1);
		aFluxH1->setDataMap(fh1);
		aFluxH3->setDataMap(fh3);
		aResL1->setDataMap(rl1);
		aResM1->setDataMap(rm1);
		aResH1->setDataMap(rh1);
		aResH3->setDataMap(rh3);
		*/
		/*
		QMap<QString, QMap<double, double> > fluxes, resolutions;
		fluxes = ((AMControlOptimizationSet*)viewSet_)->collapseAt(0, contextParams);
		resolutions = ((AMControlOptimizationSet*)viewSet_)->collapseAt(1, contextParams);
		aFluxL1->setDataMap(fluxes.value("LEG1"));
		aFluxM1->setDataMap(fluxes.value("MEG1"));
		aFluxH1->setDataMap(fluxes.value("HEG1"));
		aFluxH3->setDataMap(fluxes.value("HEG3"));
		aResL1->setDataMap(resolutions.value("LEG1"));
		aResM1->setDataMap(resolutions.value("MEG1"));
		aResH1->setDataMap(resolutions.value("HEG1"));
		aResH3->setDataMap(resolutions.value("HEG3"));

		double stepSize = 250/(numPoints-1);
		QMap<double, double> leg, meg, heg1, heg3;
		for(double x = stepSize; x < 250; x+=stepSize){
			leg.insert(resolutions.value("LEG1").value(x), fluxes.value("LEG1").value(x));
			meg.insert(resolutions.value("MEG1").value(x), fluxes.value("MEG1").value(x));
			heg1.insert(resolutions.value("HEG1").value(x), fluxes.value("HEG1").value(x));
			heg3.insert(resolutions.value("HEG3").value(x), fluxes.value("HEG3").value(x));
		}
		LEG->setDataMap(leg);
		MEG->setDataMap(meg);
		HEG1->setDataMap(heg1);
		HEG3->setDataMap(heg3);
		*/
		LEG->setDataMap(((AMControlOptimizationSet*)viewSet_)->plotAgainst(contextParams).value("LEG1"));
		MEG->setDataMap(((AMControlOptimizationSet*)viewSet_)->plotAgainst(contextParams).value("MEG1"));
		HEG1->setDataMap(((AMControlOptimizationSet*)viewSet_)->plotAgainst(contextParams).value("HEG1"));
		HEG3->setDataMap(((AMControlOptimizationSet*)viewSet_)->plotAgainst(contextParams).value("HEG3"));

		((AMControlOptimizationSet*)viewSet_)->onePlot(contextParams);
	}

protected:
	MPlot *plot, *plot2, *plot3, *plot4, *plot5;
	MPlotRealtimeModel *data1;
	MPlotRealtimeModel *data2;
	QList<MPlotRealtimeModel*> allData, allData2, resolutionLowData, resolutionMediumData, fluxLowData, fluxMediumData;
	QList<MPlotRealtimeModel*> mFluxL1, mFluxM1, mFluxH1, mFluxH3, mResL1, mResM1, mResH1, mResH3;
	QList<AMQuickDataSet*> fluxL1, fluxM1, fluxH1, fluxH3, resL1, resM1, resH1, resH3;
	AMQuickDataSet *aFluxL1, *aFluxM1, *aFluxH1, *aFluxH3, *aResL1, *aResM1, *aResH1, *aResH3;
	MPlotRealtimeModel *amFluxL1, *amFluxM1, *amFluxH1, *amFluxH3, *amResL1, *amResM1, *amResH1, *amResH3;
	AMQuickDataSet *LEG, *MEG, *HEG1, *HEG3;
	MPlotRealtimeModel *mLEG, *mMEG, *mHEG1, *mHEG3;
	int numPoints;
	bool firstTime;
};

class AMCompactControlOptimizationSetView : public QGroupBox
{
	Q_OBJECT
public:
	AMCompactControlOptimizationSetView(AMControlSet *viewSet, QWidget *parent = 0);

protected slots:
	//bool launchDetails();
	bool adjustSlider(int val);

protected:
	AMControlOptimizationSet *viewSet_;
	QPushButton *launchDetailButton_;
	QLabel *optValue_;
	QList<QWidget*> controlBoxes_;
	QHBoxLayout *hl_;
	AMControlOptimizationSetView *detailView_;
	double maxUpper, minUpper, maxLower, minLower;
};

class AMColorControlOptimizationSetView : public QGroupBox
{
	Q_OBJECT
public:
	AMColorControlOptimizationSetView(AMControlSet *viewSet, QWidget *parent = 0);

protected slots:
	//bool launchDetails();
	bool adjustSlider(int val);

protected:
	AMControlOptimizationSet *viewSet_;
	QPushButton *launchDetailButton_;
	QLabel *optValue_;
	QList<QWidget*> controlBoxes_;
	QHBoxLayout *hl_;
	AMControlOptimizationSetView *detailView_;
	double maxUpper, minUpper, maxLower, minLower;
};

class AMAbstractDetectorSetView : public QGroupBox
{
	Q_OBJECT
public:
	AMAbstractDetectorSetView(AMAbstractDetectorSet *viewSet, QWidget *parent = 0);

	QWidget* boxByName(const QString &name){
		return detectorBoxes_.at(viewSet_->indexOf(name));
	}

	QWidget* boxAt(int row){
		return detectorBoxes_.at(row);
	}

protected:
	AMAbstractDetectorSet *viewSet_;
	QList<QWidget*> detectorBoxes_;
	QHBoxLayout *hl_;
};

#endif // AMCONTROLSETVIEW_H
