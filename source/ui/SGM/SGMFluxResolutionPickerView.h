#ifndef SGMFLUXRESOLUTIONPICKERVIEW_H
#define SGMFLUXRESOLUTIONPICKERVIEW_H

#include <QGroupBox>
#include "dataman/info/AMControlInfoList.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class AMExtendedControlEditor;
class AMXASRegionsList;

class SGMFluxResolutionPickerStaticView : public QGroupBox{
Q_OBJECT
public:
 	virtual ~SGMFluxResolutionPickerStaticView();
	SGMFluxResolutionPickerStaticView(AMXASRegionsList *regions, QWidget *parent = 0);

public slots:
	virtual void setFromInfoList(const AMControlInfoList &infoList);

protected:
	AMXASRegionsList *regions_;
	double minEnergy_;
	double maxEnergy_;

	QGroupBox *exitSlitGapGroupBox_;
	QLineEdit *exitSlitGapLineEdit_;
	QGroupBox *gratingGroupBox_;
	QLineEdit *gratingLineEdit_;
	QGroupBox *harmonicGroupBox_;
	QLineEdit *harmonicLineEdit_;

	QVBoxLayout *buttonsVL_;
	QVBoxLayout *ceVL_;
	QHBoxLayout *settingsHL_;
	QVBoxLayout *mainVL_;
};

//class SGMFluxResolutionPickerView : public QGroupBox{
class SGMFluxResolutionPickerView : public SGMFluxResolutionPickerStaticView
{
Q_OBJECT
public:
 	virtual ~SGMFluxResolutionPickerView();
	SGMFluxResolutionPickerView(AMXASRegionsList *regions, QWidget *parent = 0);

public slots:
	virtual void setFromInfoList(const AMControlInfoList &infoList);

	void setDisabled(bool disabled);

protected slots:
	void onRegionsChanged();
	void onSetpointsChanged();

	void onBestFluxButtonClicked();
	void onBestResolutionButtonClicked();

signals:
	void configValuesChanged(AMControlInfoList);

protected:
	/*
	AMXASRegionsList *regions_;
	double minEnergy_;
	double maxEnergy_;
	*/

	QPushButton *bestFluxButton_;
	QPushButton *bestResolutionButton_;
	AMExtendedControlEditor *exitSlitGapCE_;
	AMExtendedControlEditor *gratingCE_;
	AMExtendedControlEditor *harmonicCE_;
	QLabel *warningsLabel_;

	/*
	QVBoxLayout *buttonsVL_;
	QVBoxLayout *ceVL_;
	QHBoxLayout *settingsHL_;
	QVBoxLayout *mainVL_;
	*/
};

#endif // SGMFLUXRESOLUTIONPICKERVIEW_H
