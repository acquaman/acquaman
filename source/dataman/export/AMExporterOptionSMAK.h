#ifndef AMEXPORTEROPTIONSMAK_H
#define AMEXPORTEROPTIONSMAK_H

#include "dataman/export/AMExporterOptionGeneralAscii.h"

/// This exporter option adds a filter that is used in a QRegExp for knowing what data sources should be added to the SMAK file.
class AMExporterOptionSMAK : public AMExporterOptionGeneralAscii
{
	Q_OBJECT

	Q_PROPERTY(QString regExpString READ regExpString WRITE setRegExpString)

public:
	/// Constructor.
	Q_INVOKABLE AMExporterOptionSMAK(QObject *parent = 0);
	/// Copy constructor.
	AMExporterOptionSMAK(const AMExporterOptionSMAK &original);
	/// Destructor.
	virtual ~AMExporterOptionSMAK();

	/// Create a new AMExporterOptionSMAK as a copy of the current state.
	virtual AMExporterOption *createCopy() const;

	/// Returns the current string used for the regular expression.
	QString regExpString() const { return regExp_; }

public slots:
	/// Sets a new regExpString.
	void setRegExpString(const QString &regExp);

protected:
	/// String that holds the regular expression.
	QString regExp_;
};

#endif // AMEXPORTEROPTIONSMAK_H
