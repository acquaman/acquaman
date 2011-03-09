#ifndef AMCONTROLOPTIMIZATIONVIEW_H
#define AMCONTROLOPTIMIZATIONVIEW_H

#include "ui/AMControlSetView.h"
#include "acquaman/AMControlOptimization.h"


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



class AMControlOptimizationSetView : public AMOldControlSetView
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



#endif // AMCONTROLOPTIMIZATIONVIEW_H
