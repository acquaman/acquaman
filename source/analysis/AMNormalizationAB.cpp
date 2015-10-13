#include "AMNormalizationAB.h"

#include "util/AMUtility.h"

AMNormalizationAB::AMNormalizationAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	data_ = 0;
	normalizer_ = 0;
	canAnalyze_ = false;
	dataName_ = "";
	normalizationName_ = "";
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();
	setState(AMDataSource::InvalidFlag);
}

AMNormalizationAB::~AMNormalizationAB()
{

}

bool AMNormalizationAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	int firstRank = dataSources.first()->rank();

	// otherwise input sources have the same rank.
	for (int i = 1; i < dataSources.count(); i++)
		if (dataSources.at(i)->rank() != firstRank)
			return false;

	return true;
}

