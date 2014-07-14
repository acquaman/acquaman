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

signals:
    void addVariable();

public slots:
    virtual void applyChanges();

protected slots:
    void onListViewSelection(QModelIndex selection);
    void removeSelectedVariable();
    void onCollectionSelection(STVariable *selection);

protected:
    STVariableCollection *variableCollection_;
    STListView *variableList_;
    STVariableEditor *variableEditor_;

};

#endif // STVARIABLECOLLECTIONEDITOR_H
