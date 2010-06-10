#ifndef AMABSTRACTDETECTOR_H
#define AMABSTRACTDETECTOR_H

#include <QObject>
#include <QStringList>

class AMAbstractDetector : public QObject
{
Q_OBJECT
public:
	AMAbstractDetector(QString name, bool isSpectralOutput = false, QObject *parent = 0);
	~AMAbstractDetector(){;}

	QString name() const { return name_;}
	bool isSpectralOutput() const { return isSpectralOutput_;}

//signals:

public slots:
	bool setName(const QString &name) { name_ = name; return true;}

protected:
	QString name_;
	bool isSpectralOutput_;
};

class AMAbstractSpectralOutputDetector : public AMAbstractDetector
{
Q_OBJECT
public:
	AMAbstractSpectralOutputDetector(QString name, int numSpectrumBins = 0, QString xElementName = "", QStringList yElementNames = QStringList(), QObject *parent = 0);

	int numSpectrumBins() const { return numSpectrumBins_; }
	QString xElementName() const { return xElementName_; }
	QStringList yElementNames() const { return yElementNames_; }

protected:
	int numSpectrumBins_;
	QString xElementName_;
	QStringList yElementNames_;
};


#endif // AMABSTRACTDETECTOR_H
