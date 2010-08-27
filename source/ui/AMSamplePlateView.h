#ifndef AMSAMPLEPLATEVIEW_H
#define AMSAMPLEPLATEVIEW_H

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QMouseEvent>
#include <QIcon>
#include <QScrollBar>
#include <QDebug>
#include <QListView>
#include <QLineEdit>
#include <QPropertyAnimation>

#include "dataman/AMSamplePlate.h"
#include "beamline/SGMBeamline.h"
#include "dataman/AMDatabase.h"
#include "AMDetailedItemDelegate.h"

class AMSampleListView;
class AMSamplePositionItemView;
class AMSamplePositionItemExpandingAdder;

class AMSamplePlateView : public QWidget
{
Q_OBJECT
public:
	explicit AMSamplePlateView(QWidget *parent = 0);

public slots:
	void setManipulator(AMControlSet *manipulator);

protected slots:
	void onLoadExistingPlate(int index);

	void onSampleTableItemUpdated(QString tableName, int id);
	void onSampleTableItemCreated(QString tableName, int id);
	void onSampleTableItemRemoved(QString tableName, int id);

	void refreshSamples();

protected:
	QLabel *plateNameLabel_;
	QLabel *loadedLabel_;
	QComboBox *existingPlates_;
	AMSampleListView *sampleListView_;
	QVBoxLayout *vl_;

	AMSamplePlate samplePlate_;
	AMControlSet *manipulator_;

	QStandardItemModel *sampleTableModel_;
	bool sampleRefreshScheduled_;
	QList<int> sampleRefreshIDs_;
	QList<int> sampleRefreshInstructions_;
};

class AMSampleListView : public QFrame
{
Q_OBJECT
public:
	AMSampleListView(AMSamplePlate *samplePlate, QStandardItemModel *sampleTableModel, QWidget *parent = 0);

public slots:
	void setManipulator(AMControlSet *manipulator);

signals:

protected slots:
	void addNewSampleToPlate(int id);
	void onSamplePositionChanged(int index);
	void onSamplePositionAdded(int index);
	void onSamplePositionRemoved(int index);

protected:
	QSize sizeHint() const;

protected:
	AMSamplePlate *samplePlate_;
	AMControlSet *manipulator_;
	QStandardItemModel *sampleTableModel_;

	QScrollArea *sa_;
	QVBoxLayout *il_;
	QFrame *saf_;
	//QPushButton *adder_;
	AMSamplePositionItemExpandingAdder *adder_;
};

class AMSamplePositionItemView : public QFrame
{
Q_OBJECT
public:
	AMSamplePositionItemView(AMSamplePosition *samplePosition, QStandardItemModel *sampleTableModel, AMControlSet *manipulator = 0, int index = -1, QWidget *parent = 0);
	int index();
	AMControlSet* manipulator();

public slots:
	void setIndex(int index);
	void setManipulator(AMControlSet* manipulator);

protected slots:
	bool onSavePositionClicked();
	bool onRecallPositionClicked();
	bool onSampleBoxIndexChanged(int index);
	bool onSampleNameChanged();

	void updateLook();
	void onSamplePositionUpdate(int index);
	void defocusItem();

protected:
	void mousePressEvent(QMouseEvent *event);

protected:
	AMSamplePosition *samplePosition_;

	AMControlSet *manipulator_;
	QStandardItemModel *sampleTableModel_;
	bool ignoreNameChanged_;
	int index_;
	bool inFocus_;

	QHBoxLayout *hl_;
	QVBoxLayout *vl_;
	QLabel *indexLabel_;
	QComboBox *sampleBox_;
	QLabel *positionLabel_;
	QPushButton *savePositionButton_;
	QPushButton *recallPositionButton_;
};

class AMTrickComboBox;

class AMSamplePositionItemExpandingAdder : public QFrame
{
Q_OBJECT
public:
	AMSamplePositionItemExpandingAdder(QStandardItemModel *sampleTableModel, QWidget *parent = 0);
	const bool expanded() const;

public slots:
	void resetAdder();

signals:
	void sampleToAddChosen(int id);

protected slots:
	void onMarkNewButtonClicked();
	void onGoNewButtonClicked();
	void onGoExistingButtonClicked();

	void shrinkBack();
	void switchBoxes();

protected:
	QStandardItemModel *sampleTableModel_;

	QPushButton *markNewButton_;
	QLineEdit *newNameEdit_;
	QLabel *newNameLabel_;
	QComboBox *chooseExistingBox_;
	//QComboBox *emptyChooseExistingBox_;
	AMTrickComboBox *emptyChooseExistingBox_;
	QPushButton *goNewButton_;
	QPushButton *goExistingButton_;
	QPushButton *cancelButton_;

	QGridLayout *gl_;
};

class AMTrickComboBox : public QComboBox
{
Q_OBJECT
public:
	AMTrickComboBox(QWidget *parent = 0);

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent *e);
};

#endif // AMSAMPLEPLATEVIEW_H
