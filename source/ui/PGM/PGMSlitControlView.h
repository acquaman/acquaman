#ifndef PGMSLITCONTROLVIEW_H
#define PGMSLITCONTROLVIEW_H

#include <QWidget>

class AMExtendedControlEditor;
class AMPVwStatusControl;

class PGMSlitControlView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMSlitControlView(QWidget *parent = 0);

signals:

public slots:

protected:
    /// Control editor for the entrance slit width
    AMExtendedControlEditor *entranceSlitPositionEditor_;

    /// Slit views for position and width.
    AMExtendedControlEditor *exitSlitAGapEditor_;
	AMExtendedControlEditor *exitSlitAPositionEditor_;
	AMExtendedControlEditor *exitSlitBGapEditor_;
	AMExtendedControlEditor *exitSlitBPositionEditor_;

};

#endif // PGMSLITCONTROLVIEW_H
