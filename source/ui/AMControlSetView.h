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
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QGraphicsItem>

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

	QMap<QString, QVariant> configValues() { return configValues_;}

signals:
	void configValuesChanged();

public slots:

protected slots:
	void onBoxUpdate(const QString& value);

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMControlSet *viewSet_;
	QMap<QString, QVariant> configValues_;
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
		while(i != dataMap_.constEnd()){
			tmpX = i.key();
			tmpY = i.value();
			if(tmpX < minX_)
				minX_ = tmpX;
			else if(tmpX > maxX_)	\
				maxX_ = tmpX;
			if(tmpY < minY_)
				minY_ = tmpY;
			else if(tmpY > maxY_)
				maxY_ = tmpY;
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
		lastContextParams_ = contextParams;
		cANSWER_ = ((AMControlOptimizationSet*)viewSet_)->cOnePlot(contextParams);
		//ANSWER->setDataMap( ((AMControlOptimizationSet*)viewSet_)->cOnePlot(contextParams)->dataMap() );
		ANSWER->setDataMap( cANSWER_->dataMap() );
	}

signals:
	void parameterValuesChanged(double param1, double param2);

protected slots:
	void onConfigValuesChanged();

protected:
	MPlot *plot;
	AMCurve *cANSWER_;
	AMQuickDataSet *ANSWER;
	MPlotRealtimeModel *mANSWER, *mCurrentPoint;
	AMRegionsList* lastContextParams_;
	int numPoints;
};

class CCOSVItem : public QGraphicsItem
{
public:
	CCOSVItem(int width, int height, QColor curveColor, bool invert = false, bool log = false);

	//void updateCurve(QMap<double, double> data);
	void updateCurve(AMCurve *dataCurve);
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	int width_;
	int height_;
	QColor curveColor_;
	AMCurve *dataCurve_;
	QPointF *curve_;
	bool invert_;
	bool log_;
};

class AMCompactControlOptimizationSetView : public QGroupBox
{
	Q_OBJECT
public:
	AMCompactControlOptimizationSetView(AMControlSet *viewSet, QWidget *parent = 0);

public slots:
	void onRegionsUpdate(AMRegionsList* contextParams);

protected slots:
	bool onParam1SliderUpdate(int val);
	bool onParam2SliderUpdate(int val);
	bool onParamValuesChanged(double param1, double param2);

	bool onConfigValuesUpdate();

protected:
	AMControlOptimizationSet *viewSet_;
	QPushButton *launchDetailButton_;
	QSlider *param1Slider, *param2Slider;
	QLabel *paramsResult_;
	QGraphicsScene *param1Scene, *param2Scene;
	QGraphicsView *param1View, *param2View;
	AMCurve *param1Curve_, *param2Curve_;
	bool param1Trigger_, param2Trigger_;
	CCOSVItem *param1Item_, *param2Item_;
	QHBoxLayout *hl_;
	QGridLayout *gl_;
	//QMap<QString, QVariant> configValues_;
	AMControlOptimizationSetView *detailView_;

	void parseConfigValues(const QStringList configList);
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
