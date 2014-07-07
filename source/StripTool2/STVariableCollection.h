#ifndef STVARIABLECOLLECTION_H
#define STVARIABLECOLLECTION_H

#include <QObject>
#include <QSignalMapper>

#include "STVariable.h"
#include "STTime.h"

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

    QModelIndex indexOf(STVariable *toMatch) const;

    QList<STVariable*> variablesWithName(const QString &name);

signals:

public slots:
    void addVariable(const QString &name);
    void deleteVariableAt(int indexToDelete);

    void setTime(STTime *time);
    void setTimeFilteringEnabled(bool isEnabled);

protected slots:
    void updateTimeValue(int newValue);
    void updateTimeUnits(STTime::Units newUnits);

protected:

protected:
    QList<STVariable*> variables_;
    STTime *time_;
    bool timeFilteringEnabled_;

};

#endif // STVARIABLECOLLECTION_H
