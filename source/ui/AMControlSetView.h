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

class AMControlOptimizationSetView : public AMControlSetView
{
	Q_OBJECT
public:
	AMControlOptimizationSetView(AMControlOptimizationSet *viewSet, QWidget *parent = 0);

public slots:
	void onRegionsUpdate(AMRegionsList* contextParams){
		QList<QVariant> stateParams, stateParams2;
		stateParams << 250.0 << 0 << 1;
		stateParams2 << 250.0 << 1 << 1;
		QMap<double, double> map1 = ((AMControlOptimizationSet*)viewSet_)->curveAt(0, stateParams, contextParams);
		QMap<double, double> map2 = ((AMControlOptimizationSet*)viewSet_)->curveAt(0, stateParams2, contextParams);

		QMap<double, double>::const_iterator i = map1.constBegin();
		while (i != map1.constEnd()) {
			data1->insertPointBack(i.key(), i.value());
			++i;
		}

		i = map2.constBegin();
		while (i != map2.constEnd()) {
			data2->insertPointBack(i.key(), i.value());
			++i;
		}
	}

protected:
	MPlotRealtimeModel *data1;
	MPlotRealtimeModel *data2;
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
