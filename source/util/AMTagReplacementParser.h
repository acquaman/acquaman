#ifndef AMTAGREPLACEMENTPARSER_H
#define AMTAGREPLACEMENTPARSER_H

#include <QString>
#include <QList>

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

- Call setInitialText() to set the input block of text and parse the tags.

- Then, go through replacementList() to examine the tags and their arguments, and provide your replacements by setting the AMTagReplacement::replacement value.

- Finally, call getReplacedText() to receive the final text, with your substitutions in-place.


<b>Performance</b>

There are a couple reasons to use AMTagReplacementParser instead of other substitution techniques (such as regular expressions).  It is quite fast, in that it only needs to pass through the input text once to parse it (ie: order O(n), where n is the number of characters in the input text).   To do the replacements, you only need to loop through the tags which actually exist in the text, instead of searching for (and potentially replacing) all possible, recognizable tags.  This makes it valuable when the input text may only include (for example) three tags, but your set of recognizable tags might include thousands.    Finally, creating the output text is done in linear time as well, with one pass through the text.

Second, AMTagReplacementParser provides information on the number of tags found, their arguments, and their positions in the text, which might be valuable in some situations.


<b>Limitations</b>
AMTagReplacementParser does not support nested tags. Tags must match the format:

<start character>tagTextNoSpaces<argument start character>free-form argument string, spaces allowed<argument end character>

or

<start character>tagTextNoSpaces

There are no error messages or parsing help provided if (for example) you accidentally forget to close an argument, etc.

*/

class AMTagReplacementParser
{
public:
	AMTagReplacementParser(const QChar& tagStartCharacter = QChar('$'), const QChar& argumentStartCharacter = QChar('['), const QChar& argumentEndCharacter = QChar(']'));


	/// Clears the list of replacements and sets the input text block
	void setInitialText(const QString& initialText);
	/// Returns the input text the parser is currently using
	QString initialText() const { return initialText_; }

	/// Use to access the list of tags found and provide your replacement values. Warning: Do not modify AMTagReplacement::position when you do this!
	AMTagReplacementList& replacementList() { return replacementList_; }

	/// Returns the final text with all replacements inserted.
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
