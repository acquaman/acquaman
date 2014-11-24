#ifndef IDEASSAMPLEMANIPULATORMOTORGROUPVIEW_H
#define IDEASSAMPLEMANIPULATORMOTORGROUPVIEW_H

#include "ui/AMMotorGroupView.h"

class IDEASSampleManipulatorMotorGroupView : public AMMotorGroupObjectView
{
Q_OBJECT
public:
    IDEASSampleManipulatorMotorGroupView(AMMotorGroupObject *motorGroupObject, QWidget *parent = 0);

	virtual ~IDEASSampleManipulatorMotorGroupView();

signals:

public slots:

};


#endif // IDEASSAMPLEMANIPULATORMOTORGROUPVIEW_H
