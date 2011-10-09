#ifndef AMSCANPARAMETERSDICTIONARY_H
#define AMSCANPARAMETERSDICTIONARY_H

#include <QObject>
#include <QHash>

class AMScan;
class AMTagReplacementParser;
class AMAbstractTagReplacementFunctor;

class AMScanParametersDictionary : public QObject
{
Q_OBJECT
public:
	AMScanParametersDictionary(QObject *parent = 0);

	/// Parse an input string for recognizable "$keyword" tags, and return a converted string. The results depend on the currentScan_ and currentDataSourceIndex_;
	QString parseKeywordString(const QString& inputString);

	/// Parse an input string and operate on either name or exportName (calls operateImplementation()). If not operating on anything, acts the same as parseKeywordString.
	QString parseKeywordStringAndOperate(const QString &inputString);

	/// Returns whether or not the dictionary can set the name of the object is operates on ($name)
	virtual bool canOperateOnName() const = 0;

	/// Returns whether or not the dictionary can set the exportName of the object it operates on ($exportName)
	virtual bool canOperateOnExportName() const = 0;

	/// Returns whether or not this dictionary is currently operating on the name field ($name). This and operatingOnExportName() are mutually exclusive.
	virtual bool operatingOnName() const;

	/// Returns whether or not this dictionary is currently operating on the exportName field ($exportName). This and operatingOnName() are mutually exclusive.
	virtual bool operatingOnExportName() const;

public slots:
	/// Sets this dictionary (if possible) to operate on the name field ($name). This is mutually exclusive to setOperatingOnExportName().
	virtual void setOperatingOnName(bool operateOneName) = 0;
	/// Sets this dictionary (if possible) to operate on the exportName field ($exportName). This is mutually exclusive to setOperatingOnName().
	virtual void setOperatingOnExportName(bool operateOnExportName) = 0;

	/// Operate again on the last input string that was parsed
	virtual void reoperate();

signals:
	/// Emitted when text is parsed by this dictionary
	void parsed(const QString &parsed);

protected:
	/// Initializes the keywordDictionary_ with the functions you see below. You can always add more directly to keywordDictionary_.
	virtual void loadKeywordReplacementDictionary();
	virtual void loadKeywordReplacementDictionaryImplementation() = 0;
	/// If your dictionary is operating on something, you should probably have some checks to make sure that you're not parsing what you're operating on
	virtual void operateImplementation(const QString &input) = 0;

	///////////////////////////////
	// functions to implement the keyword replacement system
	///////////////////////////////

	virtual QString krName(const QString& arg = QString());
	virtual QString krTechnique(const QString &arg = QString());
	virtual QString krNumber(const QString& arg = QString());
	virtual QString krDate(const QString& arg = QString());
	virtual QString krTime(const QString& arg = QString());
	virtual QString krDateTime(const QString& arg = QString());

	virtual QString krRun(const QString& arg = QString());
	virtual QString krRunName(const QString& arg = QString());
	virtual QString krRunStartDate(const QString& arg = QString());
	virtual QString krRunEndDate(const QString& arg = QString());

	virtual QString krFacilityName(const QString& arg = QString());
	virtual QString krFacilityDescription(const QString& arg = QString());

	virtual QString krScanConfiguration(const QString& propertyName);

	virtual QString krSample(const QString& arg = QString());
	virtual QString krSampleName(const QString& arg = QString());
	virtual QString krSampleElements(const QString& arg = QString());
	virtual QString krSampleCreationDate(const QString& arg = QString());

	virtual QString krExportName(const QString& arg = QString());

protected:
	/// A dictionary of function pointers we've built to support the "$keyword" replacement system. The functors return the replacement text for a given keyword tag, possibly depending on the argument.  If you want to add more keywords to the parsing system, add them to this.
	QHash<QString, AMAbstractTagReplacementFunctor*> keywordDictionary_;

	AMTagReplacementParser* keywordParser_;

	bool operatingOnName_;
	bool operatingOnExportName_;
	bool operatingNow_;

	/// The last string that was parsed
	QString lastInputString_;
};

#endif // AMSCANPARAMETERSDICTIONARY_H
