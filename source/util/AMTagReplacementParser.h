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


#ifndef AMTAGREPLACEMENTPARSER_H
#define AMTAGREPLACEMENTPARSER_H

#include <QString>
#include <QList>
#include <QHash>


/// This is the base class for functors (function objects) which take an (optional, possibly blank) argument and return a converted string. It supports advanced usage of AMTagReplacementParser by serving as entries in a hash-table which maps tags to the functions that return their replacement values. (See AMTagReplacementParser::replaceAllUsingDictionary).
class AMAbstractTagReplacementFunctor {
public:
	virtual ~AMAbstractTagReplacementFunctor() {}

	/// Return replacement text, possibly based on an option \c argument
	virtual QString getText(const QString& argument) = 0;
};

/// This is the template class for functors (function objects) which take an (optional, possibly blank) argument and return a converted string. It supports advanced usage of AMTagReplacementParser by serving as entries in a hash-table which maps tags to the functions that return their replacement values. (See AMTagReplacementParser::replaceAllUsingDictionary).   This template version allows you to encapsulate any member function of the correct type from any object.
template <class TClass> class AMTagReplacementFunctor : public AMAbstractTagReplacementFunctor {

public:
	/// Construct a functor using a pointer to your \c object, and a pointer to a getText()-type member function \c function that eats and returns a QString
	AMTagReplacementFunctor(TClass* object, QString(TClass::*function)(const QString&)) {
		object_ = object;
		function_ = function;
	}

	/// getText() function call: calls the encapsulated member function
	virtual QString getText(const QString& argument) {
		return (*object_.*function_)(argument);
	}

private:
	TClass* object_;                  ///< pointer to object
	QString (TClass::*function_)(const QString&);   ///< pointer to getText()-type member function
};



/// This helper struct to AMTagReplacementParser provides information about a tag and its location in the text.  It's also modified by the programmer to provided the replacement value.
class AMTagReplacement {
public:
	int position;
	QString tag;
	QString arguments;
	QString replacement;
};

typedef QList<AMTagReplacement> AMTagReplacementList;


/// This class provides limited parsing and replacement of "tags" within a block of text.
/*! Tags are designated by a start character (default: "$") and can optionally have an argument immediately following the tag (delimited by "[" and "]" by default.)   For example:

\code
Today is $date and I am feeling $mood.

The weather is $weather and I will wear my $shirtColor[3] shirt today.
\endcode

There are three steps to using AMTagReplacementParser:

- 1. Call setInitialText() to set the input block of text and parse the tags.

- 2. Then, go through replacementList() to examine the tags and their arguments, and provide your replacements by setting the AMTagReplacement::replacement value. (Advanced usage: Alternatively, you can call replaceAllUsingDictionary() if you have a special hash-table of functors all ready to use to conduct the replacements.)

- 3. Finally, call getReplacedText() to receive the final text, with your substitutions in-place.



<b>Performance</b>

There are a couple reasons to use AMTagReplacementParser instead of other substitution techniques (such as regular expressions).  It is quite fast, in that it only needs to pass through the input text once to parse it (ie: order O(n), where n is the number of characters in the input text).   To do the replacements, you only need to loop through the tags which actually exist in the text, instead of searching for (and potentially replacing) all possible, recognizable tags.  This makes it valuable when the input text may only include (for example) three tags, but your set of recognizable tags might include thousands.    Finally, creating the output text is done in linear time as well, with one pass through the text.

Second, AMTagReplacementParser provides information on the number of tags found, their arguments, and their positions in the text, which might be valuable in some situations.


<b>Limitations</b>

1. AMTagReplacementParser does not support nested tags.

2. Tags must match the format:

\code
{start character}tagTextAlphaNumericNoSpaces{argument start character}free-form argument string, spaces allowed{argument end character}
\endcode

or

\code
{start character}tagTextAlphaNumericNoSpaces
\endcode

3. There are no error messages or parsing help provided if (for example) you accidentally forget to close an argument, etc.

*/

class AMTagReplacementParser
{
public:
 	virtual ~AMTagReplacementParser();
	AMTagReplacementParser(const QChar& tagStartCharacter = QChar('$'), const QChar& argumentStartCharacter = QChar('['), const QChar& argumentEndCharacter = QChar(']'));


	/// Step 1: Clears the list of replacements and sets the input text block
	void setInitialText(const QString& initialText);
	/// Returns the input text the parser is currently using
	QString initialText() const { return initialText_; }

	/// Step 2: Use to access the list of tags found and provide your replacement values. Warning: Do not modify AMTagReplacement::position when you do this!
	AMTagReplacementList& replacementList() { return replacementList_; }

	/// Step 2: Alternate: determine all of the replacement values using a dictionary of functors that return the replacement for each possible tag.
	/*! What's going on here?

It's just like using a dictionary.  We lookup the tag in the dictionary, but instead of simply finding a piece of static text, we find a pointer to an object that we can call AMTagReplacementFunctor::getText() on, possibly including the argument from the tag as payload.*/
	void replaceAllUsingDictionary(const QHash<QString, AMAbstractTagReplacementFunctor*>& lookupDictionary);

	/// Step 3: Returns the final text with all replacements inserted.
	QString getReplacedText();

protected:
	/// A data structure to represent all the tags found within the initialText(), their position, and what they should be replaced by.
	AMTagReplacementList replacementList_;
	/// The initial (pre-replaced) text
	QString initialText_;
	/// The initial text, stripped of all tags.  The replacements will be re-inserted into the positions they store with them, when getReplacedText() is called.
	QString strippedText_;

	/// Character to designate the beginning of a tag.  "$" by default.
	QChar tagStart_;
	/// Character to designate the beginning of an argument.  "[" by default.
	QChar argumentStart_;
	/// Character to designate the end of an argument.  "]" by default.
	QChar argumentEnd_;


	/// Used internally in parsing to represent where the parser is in the stream
	enum ParseState { NormalText, InsideTag, InsideArg };
	/// Used internally in parsing to represent where the parser is in the stream
	ParseState parseState_;
	/// Used internally in parsing to store the data for the currently-parsed tag
	AMTagReplacement currentTag_;


	/// Used internally in parsing
	void startArgument();
	/// Used internally in parsing
	void endArgument();
	/// Used internally in parsing
	void startTag();
	/// Used internally in parsing
	void endTag();


};

#endif // AMTAGREPLACEMENTPARSER_H
