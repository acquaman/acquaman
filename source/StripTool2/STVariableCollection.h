#ifndef STVARIABLECOLLECTION_H
#define STVARIABLECOLLECTION_H

#include <QObject>
#include <QSignalMapper>

#include "STVariable.h"

class STVariableCollection : public QObject
{
    Q_OBJECT

public:
    explicit STVariableCollection(QObject *parent = 0);
    virtual ~STVariableCollection();

    QList<STVariable*> variables() const;

    int variableCount() const;

    STVariable* variableAt(int index) const;

    int indexOf(STVariable *toMatch) const;

    QList<STVariable*> variablesWithName(const QString &name);

signals:
    void variableAdded(bool succeeded);
    void variableConnected(bool connected, int index);
    void variableDeleted(bool succeeded);

public slots:
    void addVariable(const QString &name);
    void deleteVariableAt(int index);

protected slots:
    void onVariableConnectedStateChanged(QObject *object);

protected:
    void connectVariable(int index);
    void disconnectVariable(STVariable *variable);

protected:
    QList<STVariable*> variables_;
    QSignalMapper* connectedMapper_;

};

#endif // STVARIABLECOLLECTION_H
