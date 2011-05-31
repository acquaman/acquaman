#ifndef XRFELEMENT_H
#define XRFELEMENT_H

#include "util/AMElement.h"

class XRFElement : public AMElement
{
	Q_OBJECT
public:
	/// Constructor.  Takes in an AMElement and builds a copy of it with the inclusion of selected lines for regions of interest.
	XRFElement(AMElement *el, QObject *parent = 0);

	/// Returns the list of checked off lines.  Returns the name of the line (ie: Ka1).
	QList<QString> linesSelected() const { return selected_; }
	/// Boolean used to know whether there are any checked lines in the element.
	bool hasSelected() const { return !selected_.isEmpty(); }

public slots:
	/// Add a line to the list.
	void addLine(QString line) { selected_.append(line); }
	/// Removes a line from the list.
	void removeLine(QString line) { selected_.removeAt(selected_.indexOf(line)); }

protected:
	/// The list of the selected lines.
	QList<QString> selected_;
};

#endif // XRFELEMENT_H
