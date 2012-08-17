#ifndef SGMPERIODICTABLEVIEW_H
#define SGMPERIODICTABLEVIEW_H

#include <QWidget>
#include <QMenu>

#include "util/SGM/SGMPeriodicTable.h"
#include "ui/util/AMPeriodicTableView.h"

class QLabel;
class QComboBox;
class QRadioButton;
class QHBoxLayout;
class QLineEdit;

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
	virtual int nextId() const;

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

class SGMFastScanParametersModificationWizardCopyDestinationSelectionPage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardCopyDestinationSelectionPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete();
	virtual void initializePage();
	virtual int nextId() const;

protected slots:
	void onDatabaseComboBoxIndexChanged(int index);

protected:
	void checkValidElementsHelper();

protected:
	QLabel *sourceDatabaseLabel_;
	QLabel *sourceElementLabel_;
	QLabel *sourceNameLabel_;

	QComboBox *databasesComboBox_;
	QComboBox *elementsComboBox_;
	QLabel *warningsLabel_;

	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
};

class SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete() const;
	virtual void initializePage();
	virtual int nextId() const;

protected slots:
	void onButtonClicked(QAbstractButton *button);

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;

	SGMEnergyPosition startPositionCopy_;
	SGMEnergyPosition middlePositionCopy_;
	SGMEnergyPosition endPositionCopy_;

	QRadioButton *shareEnergyPositionsButton_;
	QRadioButton *createNewEnergyPositionsButton_;
	QButtonGroup *shareOrCreateButtonGroup_;
	QHBoxLayout *masterHL_;
};

class SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete() const;
	virtual void initializePage();
	virtual int nextId() const;

protected slots:
	void onNewNameLineEditTextEdited(const QString &text);

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;

	QLineEdit *newNameLineEdit_;
	QStringList takenNames_;
};

class SGMFastScanParametersModificationWizardCopyEditScanInfoPage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardCopyEditScanInfoPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete();
	virtual void initializePage();
	virtual int nextId() const;

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;
};

class SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete();
	virtual void initializePage();
	virtual int nextId() const;

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;
};

class SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage : public QWizardPage
{
Q_OBJECT
public:
	SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete();
	virtual void initializePage();
	virtual int nextId() const;

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;
};

class SGMFastScanParametersModificationWizard : public QWizard
{
Q_OBJECT
public:
	enum { PeriodicTablePage = 0,
		EditOrCopyPage = 1,
		EditPage = 2,
		CopyDestinationSelectionPage = 3,
		ShareEnergyPositionsPage = 4,
		EditScanInfoPage = 5,
		EditFastScanParametersNamePage = 6,
		EditEnergyPositionsPage = 7,
		EditFastScanSettingsPage = 8
	     };

	SGMFastScanParametersModificationWizard(QWidget *parent = 0);

	SGMFastScanParameters* originatingFastScanParameters();
	SGMFastScanParameters* newFastScanParameters();

	AMDatabase* newDatabase();
	const AMElement* newElement();

public slots:
	void setOriginatingFastScanIndex(int indexOfFastScan);
	void copyOriginalFastScanParametersToNew(AMDatabase *database, const AMElement *element);

protected:
	int indexOfOriginatingFastScan_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;
	AMDatabase *newDatabase_;
	const AMElement *newElement_;
};

#endif // SGMPERIODICTABLEVIEW_H
