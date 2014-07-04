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
