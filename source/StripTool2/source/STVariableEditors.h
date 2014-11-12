#ifndef STVARIABLEEDITORS_H
#define STVARIABLEEDITORS_H

#include <QTabWidget>

#include "STVariable.h"
#include "STVariableEditor.h"
#include "STVariableEditorBasic.h"
#include "STVariableEditorPlot.h"
#include "STVariableEditorAdvanced.h"

class STVariableEditors : public QTabWidget
{
    Q_OBJECT

public:
    explicit STVariableEditors(STVariable *toEdit, QWidget *parent = 0);
    virtual ~STVariableEditors();

    QList<STVariableEditor*> editors() const;

    STVariableEditor* basicEditor() const;
    STVariableEditor* plotEditor() const;
    STVariableEditor* advancedEditor() const;

signals:
    void removeButtonClicked();

public slots:
    void setVariable(STVariable *newVariable);
    void applyChanges();

protected:
    STVariableEditorBasic *basicEditor_;
    STVariableEditorPlot *plotEditor_;
    STVariableEditorAdvanced *advancedEditor_;

};

#endif // STVARIABLEEDITORS_H
