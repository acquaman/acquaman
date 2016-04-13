#include "AMTESTDataModel.h"

AMTESTDataModel::AMTESTDataModel(const QString &name, QObject *parent)
	: QObject(parent)
{
	name_ = name;
}

AMTESTDataModel::~AMTESTDataModel()
{

}
