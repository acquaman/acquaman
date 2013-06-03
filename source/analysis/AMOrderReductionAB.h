#ifndef AMORDERREDUCTIONAB_H
#define AMORDERREDUCTIONAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/*!
	This analysis block accepts a single data source and is capable of reducing the order
	of a particular scan dimension.  For example, if you had a 3D scan (scanRank() == 3)
	this analysis block would provide
  */
class AMOrderReductionAB
{
public:
	AMOrderReductionAB();
};

#endif // AMORDERREDUCTIONAB_H
