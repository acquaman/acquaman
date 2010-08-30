#ifndef SGMSIDEBAR_H
#define SGMSIDEBAR_H

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>

#include "NumericControl.h"
#include "beamline/SGMBeamline.h"

class SGMSidebar : public QWidget
{
	Q_OBJECT
public:
	explicit SGMSidebar(QWidget *parent = 0);

signals:

public slots:

protected:
	QGroupBox *mainBox_;

	QVBoxLayout *mainLayout_;
	QVBoxLayout *vl_;

	AMControlEdit *energyNC_;
	AMControlEdit *gratingNC_;
	AMControlEdit *readyLabel_;
	AMControlEdit *moveStatusLabel_;
};

#endif // SGMSIDEBAR_H
