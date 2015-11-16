#include "AMScalerAnalysisTest.h"

#include "util/AMScalerCountAnalyser.h"
#include <QDebug>
AMScalerAnalysisTest::AMScalerAnalysisTest(QObject *parent) :
    QObject(parent)
{

	AnalyseVector(QVector<int>() << 0 << 10 << 15 << 20 << 20 << 20 << 20 << 15 << 10 << 0);
	AnalyseVector(QVector<int>() << 0 << 30 << 30 << 30 << 30 << 0);
	AnalyseVector(QVector<int>() << 0 << 30 << 30 << 30 << 10 << 10);
	AnalyseVector(QVector<int>() << 10 << 10 << 30 << 30 << 30 << 0 << 0 << 0);
	AnalyseVector(QVector<int>() << 0 << 0 << 30 << 30 << 0 << 0 << 30 << 30 << 30 << 30 << 0 << 0);
	AnalyseVector(QVector<int>() << 0 << 0 << 5 << 10 << 15 << 20 << 15 << 10 << 5 << 0 << 10 << 20 << 30 << 30 << 30 << 30 << 20 << 10 << 0 << 0);
	AnalyseVector(QVector<int>() << 0 << 0 << 5 << 10 << 15 << 10 << 5 << 0 << 5 << 10 << 5 << 0 << 10 << 20 << 30 << 20 << 10 << 0 << 0 );
	AnalyseVector(QVector<int>() << 0 << 0 << 10 << 20 << 30 << 20 << 10 << 0 << 0);

}

void AMScalerAnalysisTest::AnalyseVector(const QVector<int> &counts)
{
	AMScalerCountAnalyser analyser(counts);
	qDebug() << "\n"<< analyser.toString();

}
