#ifndef SGMBEAMLINEATAGLANCEBAR_H
#define SGMBEAMLINEATAGLANCEBAR_H

#include <QFrame>
#include <QHBoxLayout>

#include "NumericControl.h"
#include "beamline/SGMBeamline.h"

class SGMBeamlineAtAGlanceBar : public QFrame
{
Q_OBJECT
public:
	explicit SGMBeamlineAtAGlanceBar(QWidget *parent = 0);

signals:

public slots:

protected:
	QHBoxLayout *hl_;

	AMControlEdit *energyNC_;
	AMControlEdit *gratingNC_;
	AMControlEdit *readyLabel_;
	AMControlEdit *moveStatusLabel_;

};

#endif // SGMBEAMLINEATAGLANCEBAR_H
