#ifndef AMSCANEXEMPLAR_H
#define AMSCANEXEMPLAR_H

#include <QObject>
#include <QDateTime>

class AMScanConfiguration;

class AMScanExemplar : public QObject
{
Q_OBJECT
public:
	AMScanExemplar(QObject *parent = 0);

	QString name() const;
	QString technique() const;
	QString number() const;
	QDateTime dateTime() const;
	QString runName() const;
	QDateTime runStartDateTime() const;
	QDateTime runEndDateTime() const;
	QString facilityName() const;
	QString facilityDescription() const;
	const AMScanConfiguration* scanConfiguration() const;
	QString sampleName() const;
	QString sampleElements() const;
	QDateTime sampleDateTime() const;
	QString exportName() const;

	AMScanExemplar& operator=(const AMScanExemplar &other);

public slots:
	void setName(const QString &name);
	void setTechnique(const QString &technique);
	void setDateTime(const QDateTime &dateTime);
	void setRunName(const QString &runName);
	void setRunStartDateTime(const QDateTime &runStartDateTime);
	void setRunEndDateTime(const QDateTime &runEndDateTime);
	void setFacilityName(const QString &facilityName);
	void setFacilityDescription(const QString &facilityDescription);
	void setScanConfiguration(const AMScanConfiguration *scanConfiguration);
	void setSampleName(const QString &sampleName);
	void setSampleElements(const QString &sampleElements);
	void setSampleDateTime(const QDateTime &sampleDateTime);
	void setExportName(const QString &exportName);

protected:
	QString name_;
	QString technique_;
	QString number_;
	QDateTime dateTime_;
	QString runName_;
	QDateTime runStartDateTime_;
	QDateTime runEndDateTime_;
	QString facilityName_;
	QString facilityDescription_;
	AMScanConfiguration const *scanConfiguration_;
	QString sampleName_;
	QString sampleElements_;
	QDateTime sampleDateTime_;
	QString exportName_;
};

#endif // AMSCANEXEMPLAR_H
