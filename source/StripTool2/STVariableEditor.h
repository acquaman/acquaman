#ifndef STVARIABLEEDITOR_H
#define STVARIABLEEDITOR_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>

#include "STVariable.h"

class STVariableEditor : public QWidget
{
    Q_OBJECT

public:
    explicit STVariableEditor(STVariable *toEdit, QWidget *parent = 0);
    virtual ~STVariableEditor();

signals:

protected slots:
    void setNameText(const QString &name);
    void setDescriptionText(const QString &description);
    void setCreationText(const QString &creation);
    void setConnectionState(bool isConnected);
    void setLatestValue(double newValue);

protected:
    void setupComponents();
    void getVariableInfo();
    void applyChangesToVariable();

protected:
    STVariable *variable_;

    QLabel *name_;
    QLineEdit *descriptionEntry_;
    QLabel *creation_;
    QLabel *connected_;
    QLabel *value_;
};

#endif // STVARIABLEEDITOR_H
