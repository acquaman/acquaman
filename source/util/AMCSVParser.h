#ifndef AMCSVPARSER_H
#define AMCSVPARSER_H

#include <QString>
#include <QStringList>

/// This class takes a string from a .csv file and returns a string list of properly parsed data.
/*!
  This requires a little bit more than a simple split(',') from QString because cell's can contain
  commas as well.  Therefore, you need to go character by character.
  */
class AMCSVParser
{
public:
	/// Constructor.  Optionally provide the string to be parsed.
	AMCSVParser(const QString &stringToParse = "");
	/// Destructor.
	virtual ~AMCSVParser(){}

	/// Returns the string to be parsed.
	const QString &csvString() const { return csvString_; }
	/// Sets the string to be parsed.
	void setCSVString(const QString &newString) { csvString_ = newString; }

	/// Returns the parsed input string as QStringList.
	QStringList parseCSV() const;

protected:
	/// The string to be parsed.
	QString csvString_;
};

#endif // AMCSVPARSER_H
