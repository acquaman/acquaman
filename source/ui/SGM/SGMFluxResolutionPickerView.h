#ifndef SGMFLUXRESOLUTIONPICKERVIEW_H
#define SGMFLUXRESOLUTIONPICKERVIEW_H

#include <QGroupBox>
#include "dataman/info/AMControlInfoList.h"

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class AMExtendedControlEditor;
class AMXASRegionsList;

class SGMFluxResolutionPickerView : public QGroupBox{
Q_OBJECT
public:
	SGMFluxResolutionPickerView(AMXASRegionsList *regions, QWidget *parent = 0);

public slots:
	void setFromInfoList(const AMControlInfoList &infoList);

	void setDisabled(bool disabled);

protected slots:
	void onRegionsChanged();
	void onSetpointsChanged();

	void onBestFluxButtonClicked();
	void onBestResolutionButtonClicked();

signals:
	void configValuesChanged(AMControlInfoList);

protected:
	AMXASRegionsList *regions_;
	double minEnergy_;
	double maxEnergy_;

	QPushButton *bestFluxButton_;
	QPushButton *bestResolutionButton_;
	AMExtendedControlEditor *exitSlitGapCE_;
	AMExtendedControlEditor *gratingCE_;
	AMExtendedControlEditor *harmonicCE_;
	QLabel *warningsLabel_;

	QVBoxLayout *buttonsVL_;
	QVBoxLayout *ceVL_;
	QHBoxLayout *settingsHL_;
	QVBoxLayout *mainVL_;

};

#endif // SGMFLUXRESOLUTIONPICKERVIEW_H
