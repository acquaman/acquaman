#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/PGM/PGMBeamline.h"

class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:

protected:
	/// create and layout the content of the persistent component
	QLayout * createPersistentLayout();
	/// create the beamline status display widget
	QWidget * createBeamlineStatusWidget();

protected:

};

#endif // PGMPERSISTENTVIEW_H
