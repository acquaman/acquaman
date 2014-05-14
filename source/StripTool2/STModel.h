#ifndef STMODEL_H
#define STMODEL_H

#include <QObject>

#include "STVariable.h"
#include "STVariableCollection.h"

class STModel : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit STModel(QObject *parent = 0);
    /// Destructor.
    ~STModel();

    /// Returns the collection of all variables.
    STVariableCollection* variables() const;

    /// Returns the collection of selected variables.
    STVariableCollection* selectedVariables() const;

signals:

public slots:
    /// Adds new named variable to the collection.
    void addVariable(STVariable *toAdd);
    /// Removes all instances of named variable from the collection.
    void removeVariable(STVariable *toRemove);
    /// Add new variable to the collection of selected variables.
    void addVariableToSelection(STVariable *toAdd);
    /// Remove variable from the collection of selected variables.
    void removeVariableFromSelection(STVariable *toRemove);
    /// Clear the collection of selected variables entirely.
    void clearSelection();

protected:
    STVariableCollection *variables_;
    STVariableCollection *selectedVariables_;

};

#endif // STMODEL_H
