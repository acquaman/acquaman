#ifndef STPLOTEDITOR_H
#define STPLOTEDITOR_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>

#include "STPlot.h"

class STPlotEditor : public QDialog
{
    Q_OBJECT

public:
    /// Constructor.
    explicit STPlotEditor(QWidget *parent = 0);
    /// Destructor.
    virtual ~STPlotEditor();

signals:

public slots:
    /// Sets the value to appear in the timeEntry widget.
    void setTimeEntry(int timeValue);
    /// Sets the current time units. Not implemented yet.
    void setTimeUnits(STPlot::TimeUnits);

protected:
    QLineEdit *timeEntry_;
    QComboBox *timeUnits_;
    QPushButton *okButton_;
    QPushButton *cancelButton_;


};

#endif // STPLOTEDITOR_H
