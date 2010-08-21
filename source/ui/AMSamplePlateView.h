#ifndef AMSAMPLEPLATEVIEW_H
#define AMSAMPLEPLATEVIEW_H

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QDebug>

#include "dataman/AMSamplePlate.h"
#include "beamline/SGMBeamline.h"

class AMSampleListView;
class AMSamplePositionItemView;

class AMSamplePlateView : public QGroupBox
{
Q_OBJECT
public:
	explicit AMSamplePlateView(QString title = "Sample Plate", QWidget *parent = 0);

signals:

public slots:

protected:
	QLabel *plateNameLabel_;
	QComboBox *existingPlates_;
	AMSampleListView *sampleListView_;
	QVBoxLayout *vl_;

	AMSamplePlate *samplePlate_;
};

class AMSampleListView : public QFrame
{
Q_OBJECT
public:
	AMSampleListView(QWidget *parent = 0);

protected:
	QScrollArea *sa_;
	QVBoxLayout *il_;
	QFrame *saf_;
};

class AMSamplePositionItemView : public QFrame
{
Q_OBJECT
public:
	AMSamplePositionItemView(AMSamplePosition *samplePosition, int index = -1, QWidget *parent = 0);
	int index();

public slots:
	void setIndex(int index);

protected slots:
	void onSamplePositionUpdate();

protected:
	AMSamplePosition *samplePosition_;
	int index_;

	QHBoxLayout *hl_;
	QLabel *indexLabel_;
	QComboBox *sampleBox_;
	QLabel *positionLabel_;
	QPushButton *savePositionButton_;
	QPushButton *recallPositionButton_;
};

#endif // AMSAMPLEPLATEVIEW_H
