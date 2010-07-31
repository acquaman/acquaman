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
		while(i != dataMap_.constEnd()){
			tmpX = i.key();
			tmpY = i.value();
			if(tmpX < minX_)
				minX_ = tmpX;
			else if(tmpX > maxX_)	/*
	setToolTip(QString("QColor(%1, %2, %3)\n%4")
			   .arg(color.red()).arg(color.green()).arg(color.blue())
			   .arg("Click and drag this color onto the robot!"));
	setCursor(Qt::OpenHandCursor);
	*/
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

/*
class AMCurve : public QObject
{
	Q_OBJECT
public:
	AMCurve(QMap<double, double> dataMap, QObject *parent = 0) : QObject(parent){
		setDataMap(dataMap);
	}

	QPair<double, double> minX() const { return minX_;}

	QPair<double, double> minY() const { return minY_;}

	QPair<double, double> maxX() const { return maxX_;}

	QPair<double, double> maxY() const { return maxY_;}

	void setDataMap(QMap<double, double> dataMap){
		dataMap_ = dataMap;
		QMap<double, double>::const_iterator i = dataMap_.constBegin();
		minX_.first = i.key();
		minX_.second = i.value();
		minY_.first = i.value();
		minY_.second = i.key();
		maxX_.first = i.key();
		maxX_.second = i.value();
		maxY_.first = i.value();
		maxY_.second = i.key();
		double tmpX, tmpY;
		while(i != dataMap_.constEnd()){
			tmpX = i.key();
			tmpY = i.value();
			//qDebug() << "Inserting " << tmpX << tmpY;
			if(tmpX < minX_.first){
				minX_.first = tmpX;
				minX_.second = tmpY;
			}
			else if(tmpX > maxX_.first){
				maxX_.first = tmpX;
				maxX_.second = tmpY;
			}
			if(tmpY < minY_.first){
				minY_.first = tmpY;
				minY_.second = tmpX;
			}
			else if(tmpY > maxY_.first){
				maxY_.first = tmpY;
				maxY_.second = tmpX;
			}
			++i;
		}
	}

	double valueAt(double x) const{
		if(dataMap_.count() == 0)
			return 0;
		if( (x < minX_.first) || (x > maxX_.first) )
			return 0;
		QMap<double, double>::const_iterator i = dataMap_.lowerBound(x);
		if(dataMap_.count() <= 3)
			return i.value();
		int seek = 0;
		++i;
		if(i == dataMap_.constEnd())
			seek = -1;
		else{
			--i;
			if(i == dataMap_.constBegin())
				seek = 2;
			else{
				--i;
				if(i == dataMap_.constBegin())
					seek = 1;
			}
		}

		i = dataMap_.lowerBound(x);
		if(seek == -1)
			--i;
		else
			for(int x = 0; x < seek; x++)
				++i;

		--i;
		--i;
		QPair<double, double> first, second, third, fourth;
		first.first = i.key();
		first.second = i.value();
		++i;
		second.first = i.key();
		second.second = i.value();
		++i;
		third.first = i.key();
		third.second = i.value();
		++i;
		fourth.first = i.key();
		fourth.second = i.value();

		int j, n;
		double xi, yi, ei, chisq;
		gsl_matrix *X, *cov;
		gsl_vector *y, *w, *c;

		n = 4;

		X = gsl_matrix_alloc (n, 4);
		y = gsl_vector_alloc (n);
		w = gsl_vector_alloc (n);

		c = gsl_vector_alloc (4);
		cov = gsl_matrix_alloc (4, 4);

		double ix[4];
		double iy[4];
		double ie[4];
		ix[0] = first.first;
		ix[1] = second.first;
		ix[2] = third.first;
		ix[3] = fourth.first;
		iy[0] = first.second;
		iy[1] = second.second;
		iy[2] = third.second;
		iy[3] = fourth.second;
		ie[0] = 0.1*iy[0];
		ie[1] = 0.1*iy[1];
		ie[2] = 0.1*iy[2];
		ie[3] = 0.1*iy[3];
		for (j = 0; j < n; j++)
		{
			xi = ix[j];
			yi = iy[j];
			ei = ie[j];

			gsl_matrix_set (X, j, 0, 1.0);
			gsl_matrix_set (X, j, 1, xi);
			gsl_matrix_set (X, j, 2, xi*xi);
			gsl_matrix_set (X, j, 3, xi*xi*xi);

			gsl_vector_set (y, j, yi);
			gsl_vector_set (w, j, 1.0/(ei*ei*ei));
		}

		gsl_multifit_linear_workspace * work
				= gsl_multifit_linear_alloc (n, 4);
		gsl_multifit_wlinear (X, w, y, c, cov,
							  &chisq, work);
		gsl_multifit_linear_free (work);

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

		double rVal = C(0) + C(1)*x + C(2)*x*x + C(3)*x*x*x;
		gsl_matrix_free (X);
		gsl_vector_free (y);
		gsl_vector_free (w);
		gsl_vector_free (c);
		gsl_matrix_free (cov);

		return rVal;
	}

	double valueAtRange(double percent){
		//qDebug() << "Percent forwards to " << minX_.first + percent*(maxX_.first-minX_.first) << " for " << percent;
		return valueAt(minX_.first + percent*(maxX_.first-minX_.first) );
	}

	QPair<double, double> valuesAtRange(double percent){
		QPair<double, double> rVal;
		rVal.first = minX_.first + percent*(maxX_.first-minX_.first);
		rVal.second = valueAt(minX_.first + percent*(maxX_.first-minX_.first) );
		return rVal;
	}

protected:
	QMap<double, double> dataMap_;
	QPair<double, double> minX_, minY_, maxX_, maxY_;
};
*/

class AMControlOptimizationSetView : public AMControlSetView
{
	Q_OBJECT
public:
	AMControlOptimizationSetView(AMControlOptimizationSet *viewSet, QWidget *parent = 0);

public slots:
	void onRegionsUpdate(AMRegionsList* contextParams){
		LEG->setDataMap(((AMControlOptimizationSet*)viewSet_)->plotAgainst(contextParams).value("LEG1"));
		MEG->setDataMap(((AMControlOptimizationSet*)viewSet_)->plotAgainst(contextParams).value("MEG1"));
		HEG1->setDataMap(((AMControlOptimizationSet*)viewSet_)->plotAgainst(contextParams).value("HEG1"));
		HEG3->setDataMap(((AMControlOptimizationSet*)viewSet_)->plotAgainst(contextParams).value("HEG3"));

		//ANSWER->setDataMap(((AMControlOptimizationSet*)viewSet_)->onePlot(contextParams));
		ANSWER->setDataMap( ((AMControlOptimizationSet*)viewSet_)->cOnePlot(contextParams)->dataMap() );
	}

protected:
	MPlot *plot5;
	AMQuickDataSet *LEG, *MEG, *HEG1, *HEG3, *ANSWER;
	MPlotRealtimeModel *mLEG, *mMEG, *mHEG1, *mHEG3, *mANSWER;
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
	QMap<QString, QVariant> configValues_;
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
