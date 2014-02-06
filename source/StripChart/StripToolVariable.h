#ifndef STRIPTOOLVARIABLE_H
#define STRIPTOOLVARIABLE_H

#include <QObject>
#include <QDateTime>
#include "dataman/datasource/AMDataSource.h"

class StripToolVariable : public QObject
{
    Q_OBJECT

public:
    explicit StripToolVariable(AMDataSource *dataSource = 0, QObject *parent = 0);
    bool operator== (const StripToolVariable &anotherVariable);

signals:
    void infoChanged();
    void nameChanged(const QString &name);
    void descriptionChanged(const QString &description);
    void unitsChanged(const QString &units);
    void propertyChanged();
    void dataSourceChanged();
    void dataSourceValueUpdate(double newValue);

protected:
    QString name() const;
    void setName(const QString &newName);

    QString description() const;
    void setDescription(const QString &newDescription);

    QString units() const;
    void setUnits(const QString &newUnits);

    bool isValid() const;

    QList<QString> getInfo();
    void setInfo(QList<QString> newProperties);

    QString creationDateTime() const;

    AMDataSource* dataSource() const;
    void setDataSource(AMDataSource* newDataSource);

private slots:
    virtual void onDataSourceChanged(AMnDIndex start, AMnDIndex end) = 0;
    void onDataSourceStateChanged(int newState);
    void onDataSourceValueChanged(AMnDIndex start, AMnDIndex end);

private:
    void setAsValid(bool isValid);

private:
    QString name_;
    QString description_;
    QString units_;
    QString creationDateTime_;
    bool isValid_;

    AMDataSource* dataSource_;

};

#endif // STRIPTOOLVARIABLE_H
