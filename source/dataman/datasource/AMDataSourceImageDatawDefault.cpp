#include "AMDataSourceImageDatawDefault.h"

AMDataSourceImageDatawDefault::AMDataSourceImageDatawDefault(const AMDataSource *dataSource, double defaultValue, QObject *parent)
	: AMDataSourceImageData(dataSource, parent)
{
	defaultValue_ = defaultValue;
}

qreal AMDataSourceImageDatawDefault::minZ() const
{
	QPoint c = count();
	qreal extreme = 1e20;

	for(int xx=0; xx<c.x(); xx++)
		for(int yy=0; yy<c.y(); yy++)
			if(value(xx, yy) < extreme && value(xx, yy) != defaultValue_)
				extreme = value(xx, yy);

	return extreme;
}
