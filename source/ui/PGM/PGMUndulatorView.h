#ifndef PGMUNDULATORVIEW_H
#define PGMUNDULATORVIEW_H

#include <QWidget>

class AMExtendedControlEditor;
class PGMUndulatorView : public QWidget
{
	Q_OBJECT
public:
	explicit PGMUndulatorView(QWidget *parent = 0);

signals:

public slots:

protected:
	AMExtendedControlEditor* undulatorGapControlEditor_;
	AMExtendedControlEditor* undulatorTrackingControlEditor_;
};

#endif // PGMUNDULATORVIEW_H
