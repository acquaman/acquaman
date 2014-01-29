#ifndef REIXSRIXSSCANCONFIGURATIONVIEW_H
#define REIXSRIXSSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

class QComboBox;
class QDoubleSpinBox;
class QTimeEdit;
class QLabel;
class QRadioButton;
class QCheckBox;
class AMSampleSelector;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QTableWidget;
class QSlider;

class MPlotWidget;
class MPlot;
class MPlotImageBasic;
class MPlotRectangle;
class MPlotSeriesBasic;
class MPlotColorMap;
class MPlotPoint;

class REIXSRIXSScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT
public:
	explicit REIXSRIXSScanConfigurationView(REIXSXESScanConfiguration* config = 0, QWidget *parent = 0);
	virtual ~REIXSRIXSScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const { return config_; }

protected:

	REIXSXESScanConfiguration* config_;


	//UI Elements

	QDoubleSpinBox* energyBox_;
	QDoubleSpinBox* slitWidthBox_;

	QDoubleSpinBox* polarizationAngleBox_;
	QComboBox* polarizationBox_;
	QLabel* polarizationAngleLabel_;

	QCheckBox* doNotClearExistingCountsOption_;
	QCheckBox* applySlitWidthBox_;
//	QCheckBox* applyEnergyBox_;
	QCheckBox* applyPolarizationBox_;



	QDoubleSpinBox* maximumTotalCounts_;
	QTimeEdit* maximumTimeEdit_;

	QLineEdit* nameEdit_;
	QSpinBox* numberEdit_;
	AMSampleSelector* sampleSelector_;
	QCheckBox* autoNamingCheckBox_;

	QSlider* enegySlider_;
	QPushButton* addEnergy_;
	QPushButton* loadXASData_;
	QTableWidget* energyList_;
	QPushButton* removeEnergy_;


	MPlotWidget* plotWidget_;
	MPlot* plot_;


signals:

public slots:

};

#endif // REIXSRIXSSCANCONFIGURATIONVIEW_H
