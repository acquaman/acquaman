#ifndef AMSAMPLEPLATEVIEW_H
#define AMSAMPLEPLATEVIEW_H

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDebug>

#include "dataman/AMSamplePlate.h"

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
	QGroupBox *sampleList_;
	QVBoxLayout *vl_;

	AMSamplePlate *samplePlate_;
};

#endif // AMSAMPLEPLATEVIEW_H
