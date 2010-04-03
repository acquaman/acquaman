#ifndef AMCONTROLSETVIEW_H
#define AMCONTROLSETVIEW_H

#include "beamline/AMControlSet.h"
#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QAbstractSpinBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

//class AMControlSetView : public QWidget
class AMControlSetView : public QGroupBox
{
Q_OBJECT
public:
	explicit AMControlSetView(AMControlSet *viewSet, QWidget *parent = 0);

signals:

public slots:

protected:
//	QGroupBox *gb_;
	QHBoxLayout *ml_;
	AMControlSet *viewSet_;
};

#endif // AMCONTROLSETVIEW_H
