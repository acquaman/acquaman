#ifndef SGMFASTSCANCONFIGURATION2013VIEW_H
#define SGMFASTSCANCONFIGURATION2013VIEW_H

class QGridLayout;
class QComboBox;
class QLabel;
class QFileDialog;
class QDoubleSpinBox;
class QSpinBox;
class QLineEdit;
class QFormLayout;

class AMTopFrame;

#include <algorithm>

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "ui/util/SGM/SGMEnergyPositionView.h"
#include "ui/util/SGM/SGMFastScanSettingsView.h"
#include "beamline/AMDetectorSelector.h"
#include "ui/beamline/AMDetectorSelectorView.h"
#include "ui/AMRegExpLineEdit.h"

class SGMFastScanConfiguration2013View : public AMScanConfigurationView
{
	Q_OBJECT
public:
 	virtual ~SGMFastScanConfiguration2013View();
	SGMFastScanConfiguration2013View(SGMFastScanConfiguration2013 *sfsc, QWidget *parent = 0);

	const AMScanConfiguration* configuration() const;

	void setDetectorSelector(AMDetectorSelector *fastDetectorSelector);

signals:

protected slots:
	void onSGMBeamlineCriticalControlsConnectedChanged();

	void onScanNameEditChanged(const QString &scanName);

	void onParametersStartPositionChanged();
	void onParametersMiddlePositionChanged();
	void onParametersEndPositionChanged();
	void onFastScanSettingsChanged();

	void onStartPositionCopyChanged();
	void onMiddlePositionCopyChanged();
	void onEndPositionCopyChanged();
	void onFastScanSettingsCopyChanged();

protected:
	SGMFastScanConfiguration2013 *cfg_;

	SGMEnergyPosition startPositionCopy_;
	SGMEnergyPosition middlePositionCopy_;
	SGMEnergyPosition endPositionCopy_;
	SGMFastScanSettings fastScanSettingsCopy_;

	SGMEnergyPositionWBeamlineView *startPositionView_;
	SGMEnergyPositionWBeamlineView *middlePositionView_;
	SGMEnergyPositionWBeamlineView *endPositionView_;
	SGMFastScanSettingsView *fastScanSettingsView_;

	////////////////////////////////////////
	// THINGS I SHOULDN'T HAVE ACCESS TO //
	//////////////////////////////////////
	AMDetectorSelector *fastDetectorSelector_;

	AMDetectorSelectorView *fastDetectorSelectorView_;

	QVBoxLayout *settingsLayout_;
	AMTopFrame *topFrame_;

	QComboBox *presetsComboBox_;
	QLabel *warningsLabel_;

	QLabel *scanNameLabel_;
	AMRegExpLineEdit *scanNameEdit_;
};

#endif // SGMFASTSCANCONFIGURATION2013VIEW_H
