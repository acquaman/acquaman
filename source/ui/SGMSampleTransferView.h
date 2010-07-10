#ifndef SGMSAMPLETRANSFERVIEW_H
#define SGMSAMPLETRANSFERVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <beamline/SGMBeamline.h>

class SGMSampleTransferView : public QWidget
{
Q_OBJECT
public:
	explicit SGMSampleTransferView(QWidget *parent = 0);

signals:

public slots:
	void setAction1Success();
	void setAction2Success();
	void setAction3Success();
	void setAction4Success();

protected:
	QVBoxLayout *vl_;
	QList<AMBeamlineActionItemView*> itemViews_;
	QGridLayout *mainLayout_;
};

#endif // SGMSAMPLETRANSFERVIEW_H
