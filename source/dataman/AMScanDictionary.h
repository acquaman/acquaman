#ifndef AMSCANDICTIONARY_H
#define AMSCANDICTIONARY_H

#include <QObject>
#include <QHash>

class AMScan;
class AMTagReplacementParser;
class AMAbstractTagReplacementFunctor;


class AMScanDictionary : public QObject
{
	Q_OBJECT
public:
	AMScanDictionary(AMScan *scan, QObject *parent = 0);

	/// Parse an input string for recognizable "$keyword" tags, and return a converted string. The results depend on the currentScan_ and currentDataSourceIndex_;
	QString parseKeywordString(const QString& inputString);

	void useAsName(const QString &name);

protected:
	/// Initializes the keywordDictionary_ with the functions you see below. You can always add more directly to keywordDictionary_.
	void loadKeywordReplacementDictionary();

	///////////////////////////////
	// functions to implement the keyword replacement system
	///////////////////////////////

	QString krName(const QString& arg = QString());
	QString krTechnique(const QString &arg = QString());
	QString krNumber(const QString& arg = QString());
	QString krNotes(const QString& arg = QString());
	QString krDate(const QString& arg = QString());
	QString krTime(const QString& arg = QString());
	QString krDateTime(const QString& arg = QString());

	QString krRun(const QString& arg = QString());
	QString krRunName(const QString& arg = QString());
	QString krRunStartDate(const QString& arg = QString());
	QString krRunEndDate(const QString& arg = QString());
	QString krRunNotes(const QString& arg = QString());

	QString krFacilityName(const QString& arg = QString());
	QString krFacilityDescription(const QString& arg = QString());

	QString krScanConfiguration(const QString& propertyName);

	QString krControl(const QString& controlName);
	QString krControlDescription(const QString& controlName);
	QString krControlValue(const QString& controlName);
	QString krControlUnits(const QString& controlName);

	QString krAllControls(const QString& arg = QString());

	QString krSample(const QString& arg = QString());
	QString krSampleName(const QString& arg = QString());
	QString krSampleElements(const QString& arg = QString());
	QString krSampleCreationDate(const QString& arg = QString());
	QString krSampleNotes(const QString& arg = QString());

protected:
	/// The scan this dictionary is going to operate on
	AMScan *scan_;

	/// A dictionary of function pointers we've built to support the "$keyword" replacement system. The functors return the replacement text for a given keyword tag, possibly depending on the argument.  If you want to add more keywords to the parsing system, add them to this.
	QHash<QString, AMAbstractTagReplacementFunctor*> keywordDictionary_;

	AMTagReplacementParser* keywordParser_;

	QString useAsName_;
	bool useAsNameEnabled_;
};

#endif // AMSCANDICTIONARY_H
