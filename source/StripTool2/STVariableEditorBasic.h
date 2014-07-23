#ifndef STVARIABLEEDITORBASIC_H
#define STVARIABLEEDITORBASIC_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>

#include "STEditor.h"
#include "STVariableEditor.h"
#include "STVariable.h"

class STVariableEditorBasic : public STVariableEditor
{
    Q_OBJECT

public:
    explicit STVariableEditorBasic(STVariable *toEdit, QWidget *parent = 0);
    virtual ~STVariableEditorBasic();

signals:
    void removeButtonClicked();

public slots:
    virtual void applyChanges();

protected slots:
    void setConnectionState(bool isConnected);
    void setLatestValue(double newValue);

    void onDescriptionEntryChanged(const QString &text);
    void onUnitsEntryChanged(const QString &text);

protected:
    virtual void getVariableInfo();
    virtual void clearVariableInfo();
    virtual void connectVariable();
    virtual void disconnectVariable();
    virtual void connectUI();
    virtual void disconnectUI();

protected:
    QLabel *name_;
    QLineEdit *descriptionEntry_;
    QLabel *creation_;
    QLabel *connected_;
    QLabel *value_;
    QLineEdit *unitsEntry_;
    QPushButton *removeButton_;

    bool descriptionEdited_;
    bool unitsEdited_;
};

#endif // STVARIABLEEDITORBASIC_H
