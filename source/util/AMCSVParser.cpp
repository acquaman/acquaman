#include "AMCSVParser.h"

AMCSVParser::AMCSVParser(const QString &stringToParse)
{
	csvString_ = stringToParse;
}

QStringList AMCSVParser::parseCSV() const
{
	QString fullRow = csvString_;
	QStringList row;

	bool waitForQuote = false;
	QString temp = "";

	for (int i = 0, size = fullRow.size(); i < size; i++){

		// Start with quote.
		if (!waitForQuote && (fullRow.at(i) == QChar(0x22))){

			waitForQuote = true;
			temp.append(fullRow.at(i));
		}

		// At the end of a quote.
		else if (waitForQuote && (fullRow.at(i) == QChar(0x22))){

			waitForQuote = false;
			temp.append(fullRow.at(i));
		}

		else if (!waitForQuote && (fullRow.at(i) == QChar(0x2C))){

			row << temp;
			temp = "";
		}

		else
			temp.append(fullRow.at(i));
	}

	return row;
}
