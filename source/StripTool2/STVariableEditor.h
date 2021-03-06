#ifndef STVARIABLEEDITOR_H
#define STVARIABLEEDITOR_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QColorDialog>
#include <QPushButton>
#include <QCheckBox>

#include "STEditor.h"
#include "STVariable.h"
#include "STColorButton.h"

class STVariableEditor : public STEditor
{
    Q_OBJECT

public:
    /// Constructor. Takes the STVariable to edit as an argument.
    explicit STVariableEditor(STVariable *toEdit, QWidget *parent = 0);
    /// Destructor.
    virtual ~STVariableEditor();

    /// Returns the variable being edited.
    STVariable *variable() const;

signals:
    void variableChanged(STVariable *newVariable);
    void removeButtonClicked();

public slots:
    void setVariable(STVariable *toEdit);
    virtual void applyChanges();

protected slots:
    void setConnectionState(bool isConnected);
    void setLatestValue(double newValue);

    void onDescriptionEntryChanged(const QString &text);
    void onUnitsEntryChanged(const QString &text);
    void onColorButtonClicked();
    void onShowMarkersChanged();

protected:
    void getVariableInfo();
    void clearVariableInfo();
    void connectVariable();
    void disconnectVariable();
    void connectUI();
    void disconnectUI();

protected:
    STVariable *variable_;

    QLabel *name_;
    QLineEdit *descriptionEntry_;
    QLabel *creation_;
    QLabel *connected_;
    QLabel *value_;
    QLineEdit *unitsEntry_;
    STColorButton *colorButton_;
    QCheckBox *showMarkers_;
    QPushButton *removeButton_;

    bool descriptionEdited_;
    bool unitsEdited_;
    bool colorEdited_;
    bool showMarkersEdited_;
};

#endif // STVARIABLEEDITOR_H
