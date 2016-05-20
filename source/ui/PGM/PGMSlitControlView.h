#ifndef PGMSLITCONTROLVIEW_H
#define PGMSLITCONTROLVIEW_H

#include <QWidget>

class QLabel;
class QLineEdit;

class AMSlitView;
class AMPVwStatusControl;

class PGMSlitControlView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMSlitControlView(QWidget *parent = 0);

signals:

public slots:
    /// Handles when entrance slit width value has been updated from pv.
    void onEntranceSlitChanged(double newValue);
    /// Handles when user enters a entrance slit width and hits return. Value gets sent to pv.
    bool setEntranceSlitValue();

protected:
    /// Label and input field for entrance width.
    QLabel *entranceLabel_;
    QLineEdit *entranceWidthEdit_;
    /// Slit views for position and width.
    AMSlitView *positionSlitView_;
    AMSlitView *widthSlitView_;

};

#endif // PGMSLITCONTROLVIEW_H
