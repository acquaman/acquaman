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

    QList<STVariable*> variablesWithName(const QString &name);

signals:
    void variableAdded(bool succeeded, const QString &name);
    void variableConnected(bool connected, const QString &name);
    void variableDeleted(bool succeeded, const QString &name);

public slots:
    void addVariable(const QString &name);
    void deleteVariable(const QString &name);

protected slots:
    void onVariableConnectedStateChanged(STVariable *variable);

protected:
    void connectVariable(STVariable *variable);
    void disconnectVariable(STVariable *variable);

protected:
    QList<STVariable*> variables_;
    QSignalMapper* connectedMapper_;

};

#endif // STVARIABLECOLLECTION_H
