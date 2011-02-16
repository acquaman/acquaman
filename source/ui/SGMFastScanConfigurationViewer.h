#ifndef SGMFASTSCANCONFIGURATIONVIEWER_H
#define SGMFASTSCANCONFIGURATIONVIEWER_H

#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>

#include <algorithm>

#include "AMScanConfigurationViewer.h"
#include "acquaman/SGM/SGMFastScanConfiguration.h"

class SGMFastScanConfigurationViewer : public AMScanConfigurationViewer
{
	Q_OBJECT
public:
	SGMFastScanConfigurationViewer(SGMFastScanConfiguration *sfsc, QWidget *parent = 0);

signals:
	void lastSettings(SGMFastScanParameters *lastSettings);

protected slots:
	void onLockdowScanning(bool isLocked, QString reason);
	void onSavePathEditingFinished();
	void onNewFinalizedSavePath(const QString& savePath);
	void onSaveDialogDirectoryChosen(const QString& savePath);

	//Cluge for now to save last settings
	void onStartScanClicked();

protected:
	AMScanConfiguration *cfg_;

	QString autoSavePath_;

	QComboBox *presetsComboBox_;
	QLabel *elementLabel_;
	QLabel *runTimeLabel_;
	QLabel *startEnergyLabel_;
	QLabel *energyMidpointLabel_;
	QLabel *endEnergyLabel_;
	QLabel *motorSettingsLabel_;
//	QLabel *scalerTimeLabel_;
	QLabel *baseLineLabel_;

	QLineEdit *elementEdit_;
	QDoubleSpinBox *runTimeDSB_;
	QDoubleSpinBox *startEnergyDSB_;
	QDoubleSpinBox *energyMidpointDSB_;
	QDoubleSpinBox *endEnergyDSB_;
	QSpinBox *motorSettingsSB_;
	//    QDoubleSpinBox *scalerTimeDSB_;
	QSpinBox *baseLineSB_;

	QLabel *saveLabel_;
	QLineEdit *saveEdit_;
	QLabel *saveFbkLabel_;

	QFileDialog *autoSaveDialog_;
	QPushButton *autoSaveDialogButton_;

	QPushButton *startScanButton_;
	QPushButton *addToQueueButton_;
	QPushButton *queueDirectorButton_;
	QGridLayout gl_;
	QFormLayout *fl_;
	QFormLayout *fl2_;
};

#endif // SGMFASTSCANCONFIGURATIONVIEWER_H
