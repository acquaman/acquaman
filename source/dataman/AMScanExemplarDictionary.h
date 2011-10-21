#ifndef AMSCANEXEMPLARDICTIONARY_H
#define AMSCANEXEMPLARDICTIONARY_H

#include "dataman/AMScanParametersDictionary.h"

class AMScanConfiguration;
class AMScanExemplar;

/// Can operate on name and exportName (but assumes $exportName can't be used in $name in addition to normal circular references)

class AMScanExemplarDictionary : public AMOldScanParametersDictionary
{
Q_OBJECT
public:
	AMScanExemplarDictionary(AMScanExemplar *exemplar, QObject *parent = 0);

	bool canOperateOnName() const;
	bool canOperateOnExportName() const;
	bool canOperate() const;

public slots:
	void setOperatingOnName(bool operatingOnName);
	void setOperatingOnExportName(bool operatingOnExportName);

protected:
	void loadKeywordReplacementDictionaryImplementation();
	void operateImplementation(const QString &input);

	QString krName(const QString& arg = QString());
	QString krTechnique(const QString &arg = QString());
	QString krNumber(const QString& arg = QString());
	QString krDate(const QString& arg = QString());
	QString krTime(const QString& arg = QString());
	QString krDateTime(const QString& arg = QString());

	QString krRun(const QString& arg = QString());
	QString krRunName(const QString& arg = QString());
	QString krRunStartDate(const QString& arg = QString());
	QString krRunEndDate(const QString& arg = QString());

	QString krFacilityName(const QString& arg = QString());
	QString krFacilityDescription(const QString& arg = QString());

	QString krScanConfiguration(const QString& propertyName);

	QString krSample(const QString& arg = QString());
	QString krSampleName(const QString& arg = QString());
	QString krSampleElements(const QString& arg = QString());
	QString krSampleCreationDate(const QString& arg = QString());

	QString krExportName(const QString &arg = QString());

protected:
	AMScanExemplar *exemplar_;
};

#endif // AMSCANEXEMPLARDICTIONARY_H
