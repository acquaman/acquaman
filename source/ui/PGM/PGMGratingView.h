#ifndef PGMGRATINGVIEW_H
#define PGMGRATINGVIEW_H

#include <QWidget>

class AMExtendedControlEditor;
class PGMGratingView : public QWidget
{
	Q_OBJECT
public:
	explicit PGMGratingView(QWidget *parent = 0);

signals:

public slots:

protected:
	AMExtendedControlEditor* gratingTrackingControlEditor_;
};

#endif // PGMGRATINGVIEW_H
