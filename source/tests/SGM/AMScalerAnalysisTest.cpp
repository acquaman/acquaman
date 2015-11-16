#include "AMScalerAnalysisTest.h"

#include "util/AMScalerCountAnalyser.h"
#include "tests/SGM/AMScalerTestData.h"

#include <QDebug>
AMScalerAnalysisTest::AMScalerAnalysisTest(QObject *parent) :
    QObject(parent)
{
	QVector<double> monoCounts = AMScalerTestData::monoData();

	// Smooth the mono data across 10 elements
	QVector<double> smoothedMonoCounts = QVector<double>(monoCounts.size(), 0);
	for (int iMonoCount = 5, lastMonoCount = monoCounts.count() - 5;
	     iMonoCount < lastMonoCount;
	     ++iMonoCount) {

		smoothedMonoCounts[iMonoCount] = qRound((monoCounts[iMonoCount - 5] +
		                                   monoCounts[iMonoCount - 4] +
		                                   monoCounts[iMonoCount - 3] +
		                                   monoCounts[iMonoCount - 2] +
		                                   monoCounts[iMonoCount - 1] +
		                                   monoCounts[iMonoCount] +
		                                   monoCounts[iMonoCount + 1] +
		                                   monoCounts[iMonoCount + 2] +
		                                   monoCounts[iMonoCount + 3] +
		                                   monoCounts[iMonoCount + 4] +
		                                   monoCounts[iMonoCount + 5]) / 10.0);

	}

	AnalyseVector(smoothedMonoCounts);
//	AnalyseVector(QVector<int>() << 0 << 10 << 15 << 20 << 20 << 20 << 20 << 15 << 10 << 0);
//	AnalyseVector(QVector<int>() << 0 << 30 << 30 << 30 << 30 << 0);
//	AnalyseVector(QVector<int>() << 0 << 30 << 30 << 30 << 10 << 10);
//	AnalyseVector(QVector<int>() << 10 << 10 << 30 << 30 << 30 << 0 << 0 << 0);
//	AnalyseVector(QVector<int>() << 0 << 0 << 30 << 30 << 0 << 0 << 30 << 30 << 30 << 30 << 0 << 0);
//	AnalyseVector(QVector<int>() << 0 << 0 << 5 << 10 << 15 << 20 << 15 << 10 << 5 << 0 << 10 << 20 << 30 << 30 << 30 << 30 << 20 << 10 << 0 << 0);
//	AnalyseVector(QVector<int>() << 0 << 0 << 5 << 10 << 15 << 10 << 5 << 0 << 5 << 10 << 5 << 0 << 10 << 20 << 30 << 20 << 10 << 0 << 0 );
//	AnalyseVector(QVector<int>() << 0 << 0 << 10 << 20 << 30 << 20 << 10 << 0 << 0);

}

void AMScalerAnalysisTest::AnalyseVector(const QVector<double> &counts)
{
	AMScalerCountAnalyser analyser(counts, 2.0, 0.5);
	qDebug() << "\n"<< analyser.toString();

}
