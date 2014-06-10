#ifndef AMCURRENTAMPLIFIERVIEW_H
#define AMCURRENTAMPLIFIERVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QToolButton>
#include <QLayout>
#include <QDebug>
#include <QMenu>

#include "beamline/AMCurrentAmplifier.h"

class AMCurrentAmplifierView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor. Takes in instance of AMCurrentAmplifier.
    explicit AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, QWidget *parent = 0);
    virtual ~AMCurrentAmplifierView();

    /// Returns the pointer to the AMCurrentAmplifier this view is visualizing.
    AMCurrentAmplifier *currentAmplifier() const;

signals:

public slots:
    void refreshView();

protected slots:
    /// Handles passing changes in the value combo box to the amplifier.
    void onValueComboBoxChanged(int index);
    /// Handles passing changes in the units combo box to the amplifier.
    void onUnitsComboBoxChanged(int index);

    void onAmplifierValueChanged(int valueIndex);

    void onMinusClicked();
    void onPlusClicked();

    virtual void onCustomContextMenuRequested(QPoint position);

protected:
    void refreshDisplayValues();
    void refreshButtons();

protected:
    /// The pointer to the current amplifier this view manages.
    AMCurrentAmplifier *amplifier_;
    /// Combo box holding the amplifier value.
    QComboBox *value_;
    /// Combo box holding the amplifier units.
    QComboBox *units_;

    /// The tool button for the minus button.
    QToolButton *minus_;
    /// The tool button for the plus button.
    QToolButton *plus_;
};

#endif // AMCURRENTAMPLIFIERVIEW_H
