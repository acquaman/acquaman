#include "AMStandardAnalysisBlock.h"

// Create a new AMAnalysisBlock. The block is also an AMDataSource of output data; \c outputName is the name for this AMDataSource.
AMStandardAnalysisBlock::AMStandardAnalysisBlock(const QString& outputName, QObject *parent)
	: AMAnalysisBlock(outputName, parent) {

}
