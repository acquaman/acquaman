#include "AMTESTSeriesDataModel.h"

AMTESTSeriesDataModel::AMTESTSeriesDataModel(const QString &name, QObject *parent)
	: AMTESTDataModel(name, parent)
{
	dataModel_ = new MPlotVectorSeriesData;
}

AMTESTSeriesDataModel::~AMTESTSeriesDataModel()
{

}

