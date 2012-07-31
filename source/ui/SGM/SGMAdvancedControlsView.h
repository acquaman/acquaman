#ifndef SGMADVANCEDCONTROLSVIEW_H
#define SGMADVANCEDCONTROLSVIEW_H

#include <QWidget>

#include "beamline/SGM/SGMBeamline.h"

class QRadioButton;
class AMExtendedControlEditor;
class QButtonGroup;
class QVBoxLayout;

class SGMAdvancedControlsView : public QWidget
{
Q_OBJECT
public:
	SGMAdvancedControlsView(QWidget *parent = 0);

protected slots:
	void onCurrentMirrorStripeChanged(SGMBeamline::sgmMirrorStripe newMirrorStripe);
	void onMirrorStripeButtonsClicked(int buttonIndex);

	void onCurrentEndstationChanged(SGMBeamline::sgmEndstation newEndstation);
	void onEndstationButtonsClicked(int buttonIndex);

protected:
	AMExtendedControlEditor *undulatorHarmonicEditor_;
	AMExtendedControlEditor *undulatorOffsetEditor_;
	AMExtendedControlEditor *mirrorStripeFeedbackEditor_;
	QButtonGroup *mirrorStripeSelectionGroup_;
	QRadioButton *selectCarbonButton_;
	QRadioButton *selectSiliconButton_;
	AMExtendedControlEditor *entranceSlitEditor_;
	QButtonGroup *endstationsAvailable_;
	QRadioButton *scientaButton_;
	QRadioButton *ssaButton_;

	QVBoxLayout *mainVL_;
};

#endif // SGMADVANCEDCONTROLSVIEW_H
