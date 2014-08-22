/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
class QDoubleSpinBox;

class SGMPeriodicTableView : public QWidget
{
Q_OBJECT

public:
 	virtual ~SGMPeriodicTableView();
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
 	virtual ~SGMFastScanParametersModificationWizardPeriodicTablePage();
	SGMFastScanParametersModificationWizardPeriodicTablePage(QWidget *parent = 0);

	virtual bool validatePage();
    virtual bool isComplete() const;
	virtual void initializePage();

protected slots:
	void onFastScanChosen(int indexOfFastScan);
	void checkIsComplete();

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMPeriodicTableView *sgmPeriodicTableView_;

	QLabel *selectedScanLabel_;

	int indexOfFastScan_;
};

class SGMFastScanParametersModificationWizardEditOrCopyPage : public QWizardPage
{
Q_OBJECT
public:
 	virtual ~SGMFastScanParametersModificationWizardEditOrCopyPage();
	SGMFastScanParametersModificationWizardEditOrCopyPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete() const;
	virtual void initializePage();
	virtual int nextId() const;

protected slots:
	void checkIsComplete();
	void onButtonClicked(QAbstractButton *button);

protected:
	QLabel *editInformationLabel_;
	QLabel *copyInformationLabel_;
	QRadioButton *editRadioButton_;
	QRadioButton *copyRadioButton_;
	QButtonGroup *editOrCopyButtonGroup_;

	SGMFastScanParametersModificationWizard *fastScanWizard_;
};

class SGMEnergyPositionView;
class SGMFastScanSettingsView;

class SGMFastScanParametersModificationWizardEditPage : public QWizardPage
{
Q_OBJECT
public:
 	virtual ~SGMFastScanParametersModificationWizardEditPage();
	SGMFastScanParametersModificationWizardEditPage(QWidget *parent = 0);

	virtual bool validatePage();
    virtual bool isComplete() const;
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
 	virtual ~SGMFastScanParametersModificationWizardCopyDestinationSelectionPage();
	SGMFastScanParametersModificationWizardCopyDestinationSelectionPage(QWidget *parent = 0);

	virtual bool validatePage();
    virtual bool isComplete() const;
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
 	virtual ~SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage();
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
 	virtual ~SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage();
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
 	virtual ~SGMFastScanParametersModificationWizardCopyEditScanInfoPage();
	SGMFastScanParametersModificationWizardCopyEditScanInfoPage(QWidget *parent = 0);

	virtual bool validatePage();
    virtual bool isComplete() const;
	virtual void initializePage();
	virtual int nextId() const;

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;

	QLineEdit *scanInfoNameLineEdit_;
	QLineEdit *scanInfoScanNameLineEdit_;
	QComboBox *edgeComboBox_;
	QDoubleSpinBox *edgeEnergySpinBox_;
};

class SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage : public QWizardPage
{
Q_OBJECT
public:
 	virtual ~SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage();
	SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage(QWidget *parent = 0);

	virtual bool validatePage();
    virtual bool isComplete() const;
	virtual void initializePage();
	virtual int nextId() const;

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;

	SGMEnergyPosition startPositionCopy_;
	SGMEnergyPosition middlePositionCopy_;
	SGMEnergyPosition endPositionCopy_;

	QHBoxLayout *masterHL_;
};

class SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage : public QWizardPage
{
Q_OBJECT
public:
 	virtual ~SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage();
	SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete() const;
	virtual void initializePage();
	virtual int nextId() const;

protected slots:
	void onAnySettingsChanged();

protected:
	SGMFastScanParametersModificationWizard *fastScanWizard_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;

	SGMFastScanSettings fastScanSettingsCopy_;
	SGMFastScanSettingsView *fastScanSettingsView_;
	bool hasUnsavedChanges_;
};

class SGMFastScanParametersModificationWizardCopyConfirmationPage : public QWizardPage
{
Q_OBJECT
public:
 	virtual ~SGMFastScanParametersModificationWizardCopyConfirmationPage();
	SGMFastScanParametersModificationWizardCopyConfirmationPage(QWidget *parent = 0);

	virtual bool validatePage();
	virtual bool isComplete() const;
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
		EditFastScanSettingsPage = 8,
		CopyConfirmationPage = 9
		 };

	enum CopyDestination{ CopySameSame = 0,
		CopySameNew = 1,
		CopyNewSame = 2,
		CopyNewNew = 3,
		CopyInvalidDestination = 4
		 };

	SGMFastScanParametersModificationWizard(QWidget *parent = 0);

	SGMFastScanParameters* originatingFastScanParameters();
	SGMFastScanParameters* newFastScanParameters();

	AMDatabase* newDatabase();
	AMElement* newElement();
	SGMElementInfo* newElementInfo();
	SGMScanInfo* newScanInfo();

	SGMFastScanParametersModificationWizard::CopyDestination copyDestinationSelection();

public slots:
	void setOriginatingFastScanIndex(int indexOfFastScan);
	void copyOriginalFastScanParametersToNew(AMDatabase *database, AMElement *element);
	void createNewElementInfo();
	void createNewScanInfo();

	void setCopyDestinationSelection(SGMFastScanParametersModificationWizard::CopyDestination copyDestinationSelection);

protected:
	int indexOfOriginatingFastScan_;
	SGMFastScanParameters *originatingFastScanParameters_;
	SGMFastScanParameters *newFastScanParameters_;
	AMDatabase *newDatabase_;
	AMElement *newElement_;
	SGMElementInfo *newElementInfo_;
	SGMScanInfo *newScanInfo_;

	SGMFastScanParametersModificationWizard::CopyDestination copyDestinationSelection_;
};

#endif // SGMPERIODICTABLEVIEW_H
