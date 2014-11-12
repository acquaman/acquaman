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
    void onNameEntryChanged();
    void onNameShownChanged();
    void onTimeValueChanged();
    void onTimeUnitsChanged();

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
    bool timeValueChanged_;
    bool timeUnitsChanged_;

};

#endif // STPLOTEDITOR_H
