#ifndef SGMFASTSCANPARAMETERSVIEW_H
#define SGMFASTSCANPARAMETERSVIEW_H

#include "ui/util/SGM/SGMEnergyPositionView.h"
#include "ui/util/SGM/SGMFastScanSettingsView.h"

class QHBoxLayout;

class SGMFastScanParametersView : public QWidget
{
Q_OBJECT
public:
	SGMFastScanParametersView(SGMFastScanParameters *fastScanParameters, QWidget *parent = 0);

protected slots:
	void onEditButtonClicked();
	void onCopyButtonClicked();

	void onSaveButtonClicked();
	void onCancelButtonClicked();

	void onStartPositionCopyChanged();
	void onMiddlePositionCopyChanged();
	void onEndPositionCopyChanged();
	void onFastScanSettingsCopyChanged();

protected:
	SGMFastScanParameters *fastScanParameters_;

	SGMEnergyPosition startPositionCopy_;
	SGMEnergyPosition middlePositionCopy_;
	SGMEnergyPosition endPositionCopy_;
	SGMFastScanSettings fastScanSettingsCopy_;

	SGMEnergyPositionView *startPositionView_;
	SGMEnergyPositionView *middlePositionView_;
	SGMEnergyPositionView *endPositionView_;
	SGMFastScanSettingsView *fastScanSettingsView_;

	QPushButton *copyButton_;
	QPushButton *editButton_;

	QPushButton *saveButton_;
	QPushButton *cancelButton_;

	QHBoxLayout *buttonsHL1_;
	QHBoxLayout *buttonsHL2_;
	QVBoxLayout *masterVL_;
};

#endif // SGMFASTSCANPARAMETERSVIEW_H
