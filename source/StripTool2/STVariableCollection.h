#ifndef STVARIABLECOLLECTION_H
#define STVARIABLECOLLECTION_H

#include <QObject>
#include <QList>

#include "STVariable.h"

class STVariableCollection : public QObject
{
    Q_OBJECT

public:
    explicit STVariableCollection(QObject *parent = 0);
    ~STVariableCollection();

    /// Returns the list of variables.
    QList<STVariable *> *variables() const;

signals:
    /// Emitted after a variable has been successfully added to the collection.
    void variableAdded();
    /// Emitted after a variable has been successfully removed from the collection.
    void variableRemoved();
    /// Emitted after all instances of a variable have been successfully removed from the collection.
    void variableAllRemoved();
    /// Emitted after the collection has been cleared of all STVariables.
    void cleared();

public slots:
    /// Adds a new STVariable instance to the collection.
    void addVariable(STVariable *toAdd);
    /// Removes an instance of an STVariable from the collection, if it exists.
    void removeVariable(STVariable *toRemove);
    /// Removes all instances of an STVariable from the collection, if they exist.
    void removeAllVariable(STVariable *toRemove);
    /// Clears the collection of all STVariable instances.
    void clearCollection();

protected:
    QList<STVariable*> *variables_;

};

#endif // STVARIABLECOLLECTION_H
