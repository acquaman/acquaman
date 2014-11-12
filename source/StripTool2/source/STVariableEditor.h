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
#include <QSpinBox>

#include "STEditor.h"
#include "STVariable.h"
#include "STColorButton.h"

class STVariableEditor : public QWidget
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
    /// Emitted when the variable for this editor has changed.
    void variableChanged(STVariable *newVariable);

public slots:
    virtual void applyChanges() {return;}
    /// Sets a new variable to be edited.
    void setVariable(STVariable *toEdit);

protected:
    virtual void getVariableInfo() {return;}
    virtual void clearVariableInfo() {return;}
    virtual void connectVariable() {return;}
    virtual void disconnectVariable() {return;}
    virtual void connectUI() {return;}
    virtual void disconnectUI() {return;}

protected:
    STVariable *variable_;
};

#endif // STVARIABLEEDITOR_H
