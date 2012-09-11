#ifndef SGMPERIODICTABLEVIEW_H
#define SGMPERIODICTABLEVIEW_H

#include <QWidget>
#include <QMenu>

#include "util/SGM/SGMPeriodicTable.h"
#include "ui/util/AMPeriodicTableView.h"

class QLabel;

class SGMPeriodicTableView : public QWidget
{
Q_OBJECT

public:
	SGMPeriodicTableView(SGMPeriodicTable *sgmPeriodicTable, QWidget *parent = 0);

signals:
	void fastScanChosen(int indexOfFastScan);

protected slots:
	void onClicked(int atomicNumber);

	void onHoveredAvailableScansMenu();
	void onTriggerAvailableScansMenu();

protected:
	SGMPeriodicTable *sgmPeriodicTable_;
	AMPeriodicTableView *periodicTableView_;

	QMenu *availableScansMenu_;
	int activeMenuData_;
};

#include <QWizard>
#include <QWizardPage>

class SGMFastScanParametersModificationWizard;

class SGMFastScanParametersModificationWizardPeriodicTablePage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardPeriodicTablePage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete();
	virtual void initializePage();

protected slots:
	void onFastScanChosen(int indexOfFastScan);
	void checkIsComplete();

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMPeriodicTableView *sgmPeriodicTableView_;

	int indexOfFastScan_;
};

class SGMFastScanParametersModificationWizardEditOrCopyPage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardEditOrCopyPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete();
	virtual void initializePage();
	virtual int nextId();

protected slots:
	void onEditButtonClicked();
	void onCopyButtonClicked();

	void checkIsComplete();

protected:
	QLabel *editInformationLabel_;
	QPushButton *chooseToEditPushButton_;
	QLabel *copyInformationLabel_;
	QPushButton *chooseToCopyPushButton_;
	QLabel *choiceLabel_;

	SGMFastScanParametersModificationWizard *fastScanWizard_;
};

class SGMEnergyPositionView;
class SGMFastScanSettingsView;

class SGMFastScanParametersModificationWizardEditPage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardEditPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete();
	virtual void initializePage();

protected slots:
	void onAnySettingsChanged();

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *fastScanParameters_;

	SGMEnergyPosition startPositionCopy_;
	SGMEnergyPosition middlePositionCopy_;
	SGMEnergyPosition endPositionCopy_;
	SGMFastScanSettings fastScanSettingsCopy_;

	SGMEnergyPositionView *startPositionView_;
	SGMEnergyPositionView *middlePositionView_;
	SGMEnergyPositionView *endPositionView_;
	SGMFastScanSettingsView *fastScanSettingsView_;

	bool hasUnsavedChanges_;
};

class SGMFastScanParametersModificationWizard : public QWizard
{
Q_OBJECT
public:
	enum { PeriodicTablePage = 0,
		EditOrCopyPage = 1,
		EditPage = 2,
		CopyPage = 3
	     };

	SGMFastScanParametersModificationWizard(QWidget *parent = 0);

	SGMFastScanParameters* originatingFastScanParameters();

public slots:
	void setOriginatingFastScanIndex(int indexOfFastScan);

protected:
	int indexOfOriginatingFastScan_;
	SGMFastScanParameters *originatingFastScanParameters_;
};

#endif // SGMPERIODICTABLEVIEW_H
