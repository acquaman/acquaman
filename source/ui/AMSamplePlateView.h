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

#include "dataman/AMSamplePlate.h"
#include "beamline/SGMBeamline.h"
#include "dataman/AMDatabase.h"

class AMSampleListView;
class AMSamplePositionItemView;

class AMSamplePlateView : public QGroupBox
{
Q_OBJECT
public:
	explicit AMSamplePlateView(QString title = "Sample Plate", QWidget *parent = 0);

public slots:
	void setManipulator(AMControlSet *manipulator);

protected slots:
	void onLoadExistingPlate(int index);

protected:
	QLabel *plateNameLabel_;
	QComboBox *existingPlates_;
	AMSampleListView *sampleListView_;
	QVBoxLayout *vl_;

	AMSamplePlate samplePlate_;
	AMControlSet *manipulator_;
};

class AMSampleListView : public QFrame
{
Q_OBJECT
public:
	AMSampleListView(AMSamplePlate *samplePlate, QWidget *parent = 0);

public slots:
	void setManipulator(AMControlSet *manipulator);

signals:

protected slots:
	void addNewSampleToPlate();
	void onSamplePositionChanged(int index);
	void onSamplePositionAdded(int index);
	void onSamplePositionRemoved(int index);

protected:
	QSize sizeHint() const;

protected:
	AMSamplePlate *samplePlate_;
	AMControlSet *manipulator_;

	QScrollArea *sa_;
	QVBoxLayout *il_;
	QFrame *saf_;
	QPushButton *adder_;
};

class AMSamplePositionItemView : public QFrame
{
Q_OBJECT
public:
	AMSamplePositionItemView(AMSamplePosition *samplePosition, AMControlSet *manipulator = 0, int index = -1, QWidget *parent = 0);
	int index();
	AMControlSet* manipulator();

public slots:
	void setIndex(int index);
	void setManipulator(AMControlSet* manipulator);

protected slots:
	bool onSavePositionClicked();
	bool onRecallPositionClicked();
	bool onSampleNameChanged();

	void updateLook();
	void onSamplePositionUpdate(int index);
	void defocusItem();

protected:
	void mousePressEvent(QMouseEvent *event);

protected:
	AMSamplePosition *samplePosition_;
	AMControlSet *manipulator_;
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

#endif // AMSAMPLEPLATEVIEW_H
