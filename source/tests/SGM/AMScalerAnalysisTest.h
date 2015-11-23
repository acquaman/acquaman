#ifndef AMSCALERANALYSISTEST_H
#define AMSCALERANALYSISTEST_H

#include <QObject>

class AMScalerAnalysisTest : public QObject
{
    Q_OBJECT
public:
    explicit AMScalerAnalysisTest(QObject *parent = 0);

signals:

public slots:
protected:

	void AnalyseVector(const QVector<double>& counts);

};

#endif // AMSCALERANALYSISTEST_H
