#ifndef STVARIABLECOLLECTIONEDITOR_H
#define STVARIABLECOLLECTIONEDITOR_H

#include <QWidget>
#include <QFrame>
#include <QPushButton>

#include "STVariableEditor.h"
#include "STVariableCollection.h"
#include "STListView.h"

class STVariableCollectionEditor : public STEditor
{
    Q_OBJECT

public:
    explicit STVariableCollectionEditor(STVariableCollection *collection, QWidget *parent = 0);
    virtual ~STVariableCollectionEditor();

    STVariableCollection* variableCollection() const;
    STVariable* selectedVariable() const;

signals:
    void selectedVariableChanged(STVariable *newSelection);
    void addVariable();

public slots:
    virtual void applyChanges();

protected slots:
    void onListViewSelection(QModelIndex selection);
    void setSelectedVariable(STVariable *newSelection);
    void onRemoveButtonClicked();

protected:
    QListView *variableList_;
    STVariableCollection *variableCollection_;
    STVariable *selectedVariable_;
    STVariableEditor *variableEditor_;

};

#endif // STVARIABLECOLLECTIONEDITOR_H
