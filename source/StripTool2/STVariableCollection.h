#ifndef STVARIABLECOLLECTION_H
#define STVARIABLECOLLECTION_H

#include <QObject>
#include <QSignalMapper>

#include "STVariable.h"

class STVariableCollection : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit STVariableCollection(QObject *parent = 0);
    virtual ~STVariableCollection();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QList<STVariable*> variables() const;

    int variableCount() const;

    STVariable* variableAt(int index) const;

    int indexOf(STVariable *toMatch) const;

    QList<STVariable*> variablesWithName(const QString &name);

signals:
    void variableAdded(int index);
    void variableConnectedStateChanged(int index);
    void variableDescriptionChanged(int index);
    void variableUnitsChanged(int index);
    void variableDeleted();

public slots:
    void addVariable(const QString &name);
    void deleteVariableAt(int index);

protected slots:
    void onVariableConnectedStateChanged(bool isConnected);
    void onVariableDescriptionChanged(const QString &description);
    void onVariableUnitsChanged(const QString &units);

protected:
    void connectVariable(STVariable *toConnect);
    void disconnectVariable(STVariable *variable);

protected:
    QList<STVariable*> variables_;
    QSignalMapper* connectedMapper_;

};

#endif // STVARIABLECOLLECTION_H
