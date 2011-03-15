#ifndef SGMFASTSCANCONFIGURATIONVIEWER_H
#define SGMFASTSCANCONFIGURATIONVIEWER_H

#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QFormLayout>

#include <algorithm>

#include "AMScanConfigurationView.h"
#include "acquaman/SGM/SGMFastScanConfiguration.h"

class SGMFastScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	SGMFastScanConfigurationView(SGMFastScanConfiguration *sfsc, QWidget *parent = 0);

	const AMScanConfiguration* configuration() const;

signals:
	/* NTBA March 14, 2011 David Chevrier
	   Part of exporter package
	void lastSettings(SGMFastScanParameters *lastSettings);
	*/

protected slots:
	void onSGMBeamlineCriticalControlsConnectedChanged();
	/* NTBA March 14, 2011 David Chevrier
	void onSavePathEditingFinished();
	void onNewFinalizedSavePath(const QString& savePath);
	void onSaveDialogDirectoryChosen(const QString& savePath);
	//Cluge for now to save last settings
	*/

protected:
	SGMFastScanConfiguration *cfg_;

	/* NTBA March 14, 2011 David Chevrier
	   Part of exporter package
	QString autoSavePath_;
	*/

	QComboBox *presetsComboBox_;
	QLabel *elementLabel_;
	QLabel *runTimeLabel_;
	QLabel *startEnergyLabel_;
	QLabel *energyMidpointLabel_;
	QLabel *endEnergyLabel_;
	QLabel *motorSettingsLabel_;
	QLabel *baseLineLabel_;
	QLabel *warningsLabel_;

	QLineEdit *elementEdit_;
	QDoubleSpinBox *runTimeDSB_;
	QDoubleSpinBox *startEnergyDSB_;
	QDoubleSpinBox *energyMidpointDSB_;
	QDoubleSpinBox *endEnergyDSB_;
	QSpinBox *motorSettingsSB_;
	QSpinBox *baseLineSB_;

	/* NTBA March 14, 2011 David Chevrier
	   Part of exporter package
	QLabel *saveLabel_;
	QLineEdit *saveEdit_;
	QLabel *saveFbkLabel_;

	QFileDialog *autoSaveDialog_;
	QPushButton *autoSaveDialogButton_;
	*/
	QGridLayout gl_;
	QFormLayout *fl_;
	QFormLayout *fl2_;
};

#endif // SGMFASTSCANCONFIGURATIONVIEWER_H
