/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMSCANPARAMETERSDICTIONARY_H
#define AMSCANPARAMETERSDICTIONARY_H

#include <QObject>
#include <QHash>

class AMTagReplacementParser;
class AMAbstractTagReplacementFunctor;

class AMScanParametersDictionary : public QObject
{
Q_OBJECT
public:
	/// Enum for what parameter this dictionary is operating on, if any
	enum OperatingParameter { OperateOnNone = 0,
				  OperateOnName = 1,
				  OperateOnExportName = 2,
				  OperateOnNotes = 3 };

 	virtual ~AMScanParametersDictionary();
	AMScanParametersDictionary(AMScanParametersDictionary::OperatingParameter operatingOn,  QObject *parent = 0);

	/// Returns the input string (the unparsed keyword string)
	const QString& input() const;

	/// Returns the output string (empty string if no input string). If underlying changes have been made, you may need to call reOperate().
	const QString& output() const;

	/// Returns the output string after making sure it is up to date (empty string if no input string). Not a const function.
	const QString& reOperateOutput();

	/// Returns whether or not this particular dictionary can operate on a given parameter (ALL dictionaries can ALWAYS operate on "None")
	virtual bool canOperateOn(AMScanParametersDictionary::OperatingParameter operatingOn) const;

	/// Return whether or not the dictionary can currently operate at all (in case your dictionary is reparsing when another object changes and you want to stop it from parsing until the end of a reload, for example)
	virtual bool canOperate() const = 0;

	/// Returns which parameter this dictionary is operating on (the default is "None")
	AMScanParametersDictionary::OperatingParameter operatingOn() const;

public slots:
	/// Sets the input string (the keyword string) and forces the dictionary to parse the output again
	virtual void setInput(const QString &input);

	/// Sets the parameter this scan is operating on (dictionaries should only respond to parameters they canOperate() on ... all dictionaries should respond to "None")
	virtual void setOperatingOn(AMScanParametersDictionary::OperatingParameter operatingOn);

	/// Forces this dictionary to parse the same input (in case the underlying parameters changed) ... most dictionaries should do this for themselves
	virtual void reOperate();

signals:
	/// Emitted when the user sets a new input (keyword) string
	void inputChanged(const QString &input);

	/// Emitted when there is a new output (either the input changed, the dictionary reparsed because an underlying parameter changed, or reoperate was called and the output changed)
	void outputChanged(const QString &output);

	/// Emitted when the user sets a new operating parameter
	void operatingParameterChanged(AMScanParametersDictionary::OperatingParameter operatingOn);

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

	/// Stores which parameter this dictionary is operating on
	AMScanParametersDictionary::OperatingParameter operatingOn_;

	/// The input string that is parsed
	QString input_;
	/// The output string after parsing
	QString output_;
};

class AMOldScanParametersDictionary : public QObject
{
Q_OBJECT
public:
 	virtual ~AMOldScanParametersDictionary();
	AMOldScanParametersDictionary(QObject *parent = 0);

	/// Parse an input string for recognizable "$keyword" tags, and return a converted string. The results depend on the currentScan_ and currentDataSourceIndex_
	QString parseKeywordString(const QString& inputString);

	/// Parse an input string and operate on either name or exportName (calls operateImplementation()). If not operating on anything, acts the same as parseKeywordString.
	QString parseKeywordStringAndOperate(const QString &inputString);

	/// Returns whether or not the dictionary can set the name of the object is operates on ($name)
	virtual bool canOperateOnName() const = 0;

	/// Returns whether or not the dictionary can set the exportName of the object it operates on ($exportName)
	virtual bool canOperateOnExportName() const = 0;

	/// Return whether or not the dictionary can currently operate at (will stop any parsing from being executed in reoperate)
	virtual bool canOperate() const = 0;

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
