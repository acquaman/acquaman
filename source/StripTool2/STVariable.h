#ifndef STVARIABLE_H
#define STVARIABLE_H

#include <QObject>
#include <QDebug>

#include "beamline/AMProcessVariable.h"
#include "analysis/AM0DAccumulatorAB.h"
#include "analysis/AM0DTimestampAB.h"
#include "analysis/AM1DTimedDataAB.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

#include "MPlot/MPlotSeries.h"

class STVariable : public QObject
{
    Q_OBJECT
public:
    explicit STVariable(const QString &name, QObject *parent = 0);
    virtual ~STVariable();

    AMProcessVariable *pv() const;
    MPlotSeriesBasic *series() const;


signals:
    void connected(bool isConnected);

public slots:

protected slots:
    void onVariableConnected(bool isConnected);

protected:
    AMProcessVariable *pv_;

    AM0DAccumulatorAB *data_;
    AM0DTimestampAB *times_;
    AM1DTimedDataAB *timedData_;

    AMDataSourceSeriesData *seriesData_;
    MPlotSeriesBasic *series_;

};

#endif // STVARIABLE_H
