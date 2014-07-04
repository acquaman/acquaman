#ifndef STPLOTEDITOR_H
#define STPLOTEDITOR_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>
#include <QDebug>

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
    /// Sets the value to appear in the nameEntry widget.
    void setNameEntry(const QString &oldName);
    /// Sets the checked state of nameShown widget.
    void setNameShown(bool oldState);
    /// Sets the value to appear in the timeEntry widget.
    void setTimeEntry(int oldValue);
//    /// Sets the current time units. Not implemented yet.
//    void setTimeUnits(STPlot::TimeUnits);

    void onNameEntryChanged();
    void onNameShownChanged();

protected:
    void getPlotInfo();
    void clearPlotInfo();

protected:
    STPlotWidget *plot_;

    QLineEdit *nameEntry_;
    QCheckBox *nameShown_;
    QLineEdit *timeEntry_;
    QComboBox *timeUnits_;

    bool nameChanged_;
    bool nameShownChanged_;

};

#endif // STPLOTEDITOR_H
