#include "STVariable.h"

STVariable::STVariable(const QString &name, QObject *parent) :
    QObject(parent)
{
    Q_UNUSED(name)

    pv_ = 0;
    data_ = 0;
    times_ = 0;

    seriesData_ = 0;
    series_ = 0;
}

STVariable::~STVariable()
{

}

AMProcessVariable* STVariable::pv() const
{
    return pv_;
}

MPlotSeriesBasic* STVariable::series() const
{
    return series_;
}

void STVariable::onVariableConnected(bool isConnected)
{
    if (isConnected)
        qDebug() << "Variable is connected.";
}
