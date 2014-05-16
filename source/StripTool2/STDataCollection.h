#ifndef STDATACOLLECTION_H
#define STDATACOLLECTION_H

#include <QObject>

#include "MPlot/MPlotSeriesData.h"

class STData;

class STDataCollection : public QObject
{
    Q_OBJECT

public:
    explicit STDataCollection(const QString &name, QObject *parent = 0);
    ~STDataCollection();

    /// Returns the name of the STDataCollection object.
    QString name() const;

signals:
    /// Notifier emitted when the name of the STDataCollection object has been changed.
    void nameChanged(const QString &newName);
    /// Notifier emitted when the data stored in STDataCollection has been changed/updated.
    void dataChanged();

public slots:
    /// Sets the name of the STDataCollection object.
    void setName(const QString &newName);

    /// Adds a new data point (time/value pair) to the STDataCollection object.
    virtual void addData(QTime &time, double value) = 0;

protected:
    /// The name of the STDataCollection object (usually the pv name of the STVariable that owns it).
    QString name_;

};

#endif // STDATACOLLECTION_H
