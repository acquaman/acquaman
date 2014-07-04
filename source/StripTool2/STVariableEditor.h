#ifndef STVARIABLEEDITOR_H
#define STVARIABLEEDITOR_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>

#include "STEditor.h"
#include "STVariable.h"

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

public slots:
    void setVariable(STVariable *toEdit);
    virtual void applyChanges();

protected slots:
    void setNameText(const QString &name);
    void setDescriptionText(const QString &description);
    void setUnitsText(const QString &units);
    void setCreationText(const QString &creation);
    void setConnectionState(bool isConnected);
    void setLatestValue(double newValue);

    void onVariableValueChanged(double newValue);

    void onDescriptionEntryChanged(const QString &text);
    void onUnitsEntryChanged(const QString &text);

protected:
    void setupComponents();
    void getVariableInfo();
    void clearVariableInfo();
    void connectVariable();
    void disconnectVariable();

protected:
    STVariable *variable_;

    QLabel *name_;
    QLineEdit *descriptionEntry_;
    QLineEdit *unitsEntry_;
    QLabel *creation_;
    QLabel *connected_;
    QLabel *value_;

    bool descriptionEdited_;
    bool unitsEdited_;
};

#endif // STVARIABLEEDITOR_H
