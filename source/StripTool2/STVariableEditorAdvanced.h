#ifndef STVARIABLEEDITORADVANCED_H
#define STVARIABLEEDITORADVANCED_H

#include <QWidget>
#include <QSpinBox>
#include <QLayout>

#include "STVariableEditor.h"

class STVariableEditorAdvanced : public STVariableEditor
{
    Q_OBJECT
public:
    explicit STVariableEditorAdvanced(STVariable *toEdit, QWidget *parent = 0);
    virtual ~STVariableEditorAdvanced();

signals:

public slots:
    virtual void applyChanges();

protected slots:
    void onPointsSavedEntryChanged();

protected:
    virtual void getVariableInfo();
    virtual void clearVariableInfo();
    virtual void connectUI();
    virtual void disconnectUI();

protected:
    QSpinBox *pointsSavedEntry_;

    bool pointsSavedEdited_;

};

#endif // STVARIABLEEDITORADVANCED_H
