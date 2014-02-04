#ifndef STRIPTOOLVARIABLE_H
#define STRIPTOOLVARIABLE_H

#include <QObject>
#include "dataman/datasource/AMDataSource.h"

class StripToolVariable : public QObject
{
    Q_OBJECT

public:
    explicit StripToolVariable(QObject *parent = 0);
    bool operator== (const StripToolVariable &anotherVariable);

signals:
    void nameChanged(const QString &name);
    void descriptionChanged(const QString &description);
    void unitsChanged(const QString &units);
//    void propertyChanged
    void checkStateChanged(const Qt::CheckState &checkState);
    void dataSourceChanged();

protected:
    QString name() const;
    void setName(const QString &newName);

    QString description() const;
    void setDescription(const QString &newDescription);

    QString units() const;
    void setUnits(const QString &newUnits);

    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState newState);

    AMDataSource* dataSource() const;
    void setDataSource(AMDataSource* newDataSource);

private:
//    virtual void createDataSource(const QString pvName) = 0;

private:
    QString name_;
    QString description_;
    QString units_;
    Qt::CheckState checkState_;
    AMDataSource* dataSource_;

};

#endif // STRIPTOOLVARIABLE_H
