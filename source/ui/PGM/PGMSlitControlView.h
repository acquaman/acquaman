#ifndef PGMSLITCONTROLVIEW_H
#define PGMSLITCONTROLVIEW_H

#include <QWidget>

class AMExtendedControlEditor;

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
	AMExtendedControlEditor *exitSlitAPositionTrackingEditor_;
	AMExtendedControlEditor *exitSlitBGapEditor_;
	AMExtendedControlEditor *exitSlitBPositionEditor_;
	AMExtendedControlEditor *exitSlitBPositionTrackingEditor_;

};

#endif // PGMSLITCONTROLVIEW_H
