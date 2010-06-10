#ifndef ACQMAN_AMDETECTOR_H
#define ACQMAN_AMDETECTOR_H

#include <QObject>
#include "AMAbstractDetector.h"
#include "AMControl.h"

class AMDetector : public AMAbstractDetector
{
	Q_OBJECT
public:
	AMDetector(QString name, bool isSpectrum, QObject *parent = 0) :
			AMAbstractDetector(name, isSpectrum, parent) {;}
};

class AMSingleControlDetector : public AMDetector
{
Q_OBJECT
public:
	AMSingleControlDetector(QString name, AMControl *control, QObject *parent = 0);// : AMDetector(name, parent) {control_ = control;}
	~AMSingleControlDetector(){ control_ = NULL;}

protected:
	AMControl *control_;
};

class AMSpectralOutputDetector : public AMAbstractSpectralOutputDetector
{
Q_OBJECT
public:
	AMSpectralOutputDetector(QString name, AMControl *control, int numSpectrumBins = 0, QString xElementName = "", QStringList yElementNames = QStringList(), QObject *parent = 0);
	~AMSpectralOutputDetector() { control_ = NULL; }

protected:
	AMControl *control_;
};

#endif // AMDETECTOR_H
