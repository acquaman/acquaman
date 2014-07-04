#ifndef STPLOTEDITOR_H
#define STPLOTEDITOR_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>

#include "STPlotWidget.h"
#include "STEditor.h"


class STPlotEditor : public STEditor
{
    Q_OBJECT

public:
    /// Constructor.
    explicit STPlotEditor(STPlotWidget *plot, QWidget *parent = 0);
    /// Destructor.
    virtual ~STPlotEditor();

signals:

public slots:
    virtual void applyChanges();

protected slots:
    /// Sets the value to appear in the timeEntry widget.
    void setTimeEntry(int timeValue);
//    /// Sets the current time units. Not implemented yet.
//    void setTimeUnits(STPlot::TimeUnits);

protected:
    STPlotWidget *plot_;

    QLineEdit *timeEntry_;
    QComboBox *timeUnits_;
    QPushButton *okButton_;
    QPushButton *cancelButton_;


};

#endif // STPLOTEDITOR_H
