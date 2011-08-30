#ifndef VESPERSDIAGNOSTICSVIEW_H
#define VESPERSDIAGNOSTICSVIEW_H

#include <QWidget>
#include <QLabel>

#include "beamline/AMControlSet.h"

/// This class is simply an element for the diagnostics view.  It handles the connection to the individual element.
class VESPERSDiagnosticsViewElement : public QWidget
{
    Q_OBJECT
public:
    /// Constructor.
    /*!
      \param control is the AMControl that this element is watching.
      \param name is the specific name this element should be using.
      */
    explicit VESPERSDiagnosticsViewElement(AMControl *control, QString name, QWidget *parent = 0);

protected slots:
    /// Handles updating the label.
    void onValueChanged(double val) { value_->setText(QString::number(val)+" "+control_->units()); }

protected:
    /// Pointer to the control.
    AMControl *control_;
    /// Value label.
    QLabel *value_;
};

/*!
  This class is a customized general view class for diagnostics on the VESPERS beamline.  It takes in a control set
  and offset and prints out the pertinent information of the beamline in a quick manner.  The purpose is to get a
  quick look at the diagnostics of the beamline without resorting to looking at an EDM screen.
  */
class VESPERSDiagnosticsView : public QWidget
{
    Q_OBJECT
public:
    /// Constructor.
    /*!
      \param set is the control set that is going to be viewed.
      \param offset is the number of extra words in front of the name of the control.
      */
    explicit VESPERSDiagnosticsView(AMControlSet *set, int offset, QWidget *parent = 0);

signals:

public slots:

};

#endif // VESPERSDIAGNOSTICSVIEW_H
