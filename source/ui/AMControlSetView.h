/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
	bool boxTrigger() const { return boxTrigger_;}

signals:
	void configValuesChanged();

public slots:
	void setConfigValues(QMap<QString, QVariant> configValues);
	void resetBoxTrigger(){boxTrigger_ = false;}

protected slots:
	void onBoxUpdate(const QString& value);

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMControlSet *viewSet_;
	QMap<QString, QVariant> configValues_;
	QList<QWidget*> controlBoxes_;
	QList<bool> dirty_;
	bool boxTrigger_;
	QHBoxLayout *hl_;
};

class AMQuickDataSet : public QObject
{
	Q_OBJECT
public:
	AMQuickDataSet(QMap<double, double> dataMap, QObject *parent = 0);

	double minY() { return minY_; }
	double maxY() { return maxY_; }
	double minX() { return minX_; }
	double maxX() { return maxX_; }

	void setModel(MPlotRealtimeModel *model);
	void setupModel();
	void setDataMap(QMap<double, double> dataMap);

protected:
	QMap<double, double> dataMap_;
	double minX_;
	double minY_;
	double maxX_;
	double maxY_;
	MPlotRealtimeModel *model_;
};

#include "acquaman/AMControlOptimization.h"

class AMControlOptimizationSetView : public AMControlSetView
{
	Q_OBJECT
public:
	AMControlOptimizationSetView(AMControlOptimizationSet *viewSet, QWidget *parent = 0);

public slots:
	void onRegionsUpdate(AMRegionsList* contextParams);

signals:
	void parameterValuesChanged(double param1, double param2);

protected slots:
	void onConfigValuesChanged();

protected:
	MPlot *plot;
	AMCurve *cANSWER_;
	AMQuickDataSet *ANSWER;
	MPlotRealtimeModel *mANSWER, *mCurrentPoint, *mVerticalCursor, *mHorizontalCursor;
	AMRegionsList* lastContextParams_;
	int numPoints;
};

class CCOSVItem : public QGraphicsItem
{
public:
	CCOSVItem(int width, int height, QColor curveColor, bool invert = false, bool log = false);

	void updateCurve(AMCurve *dataCurve);
	void updateCurveMarker(double percent);
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	int width_;
	int height_;
	QColor curveColor_;
	AMCurve *dataCurve_;
	double curveMarker_;
	QPointF *curve_;
	bool invert_;
	bool log_;
};

class AMCompactControlOptimizationSetView : public QGroupBox
{
	Q_OBJECT
public:
	AMCompactControlOptimizationSetView(AMControlSet *viewSet, QWidget *parent = 0);

	AMControlOptimizationSetView *detailView() { return detailView_;}

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
	bool param1Trigger_, param2Trigger_, slider1Trigger_, slider2Trigger_, details1Trigger_, details2Trigger_;
	bool optimumValues_;
	CCOSVItem *param1Item_, *param2Item_;
	QHBoxLayout *hl_;
	QGridLayout *gl_;
	AMControlOptimizationSetView *detailView_;

	void parseConfigValues(const QStringList configList);
	void nonOptimumValues(bool optimumValues);
};

class AMColorControlOptimizationSetView : public QGroupBox
{
	Q_OBJECT
public:
	AMColorControlOptimizationSetView(AMControlSet *viewSet, QWidget *parent = 0);

protected slots:
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

class AMDetectorInfoView : public QGroupBox
{
	Q_OBJECT
public:
	AMDetectorInfoView(AMDetectorInfo *detectorInfo, AMDetectorInfo *writeDetectorInfo = 0, bool interactive = false, QWidget *parent = 0);

protected:
	AMDetectorInfo *detectorInfo_;
	AMDetectorInfo *writeDetectorInfo_;
	QPushButton *switchToEditBox_;
	bool interactive_;
	QHBoxLayout *hl_;
	QVBoxLayout *vl_;
};

class PGTDetectorInfoView : public AMDetectorInfoView
{
	Q_OBJECT
public:
	PGTDetectorInfoView(PGTDetectorInfo *detectorInfo, AMDetectorInfo *writeDetectorInfo = 0, bool interactive = false, QWidget *parent = 0);

protected:
	PGTDetectorInfo *sDetectorInfo_;
	QDoubleSpinBox *integrationTimeBox_;
	QComboBox *integrationModeBox_;
	QDoubleSpinBox *hvSetpointBox_;
	QList<QWidget*> allBoxes_;
	QFormLayout *fl_;
};

class MCPDetectorInfoView : public AMDetectorInfoView
{
	Q_OBJECT
public:
	MCPDetectorInfoView(MCPDetectorInfo *detectorInfo, AMDetectorInfo *writeDetectorInfo = 0, bool interactive = false, QWidget *parent = 0);

protected:
	MCPDetectorInfo *sDetectorInfo_;
	QDoubleSpinBox *hvSetpointBox_;
	QList<QWidget*> allBoxes_;
	QFormLayout *fl_;
};

#include "acquaman/AMDetectorInfoList.h"

class AMDetectorInfoSetView : public QGroupBox
{
	Q_OBJECT
public:
	AMDetectorInfoSetView(AMDetectorInfoSet *viewSet, AMDetectorInfoSet *writeSet = 0, bool setup = true, QWidget *parent = 0);

	QWidget* boxByName(const QString &name){
		return detectorBoxes_.at(viewSet_->indexOf(name));
	}

	QWidget* boxAt(int row){
		return detectorBoxes_.at(row);
	}

protected:
	AMDetectorInfoSet *viewSet_;
	AMDetectorInfoSet *writeSet_;
	QList< QWidget* > detectorBoxes_;
	QList< QWidget* > detectorDetails_;
	QList< QWidget* > detailViews_;
	QHBoxLayout *hl_;

	virtual void runSetup();
	virtual QWidget* detailViewByType(AMDetectorInfo *detectorInfo, AMDetectorInfo *writeDetectorInfo);
};

#endif // AMCONTROLSETVIEW_H
