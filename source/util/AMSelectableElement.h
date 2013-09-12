#ifndef AMSELECTABLEELEMENT_H
#define AMSELECTABLEELEMENT_H

#include "util/AMElement.h"

/// This class extends the basic AMElement class to provide a means of "choosing" or "selecting" emission lines and absorption edges and keeping a memory of them.
class AMSelectableElement : public AMElement
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid element name (eg: Iron).  Will become a null object if the name is invalid.
	AMSelectableElement(const QString &elementName, QObject *parent = 0);
	/// Constructor.  Takes an existing AMElement and builds an AMSelectableElement out of it.
	AMSelectableElement(const AMElement &element, QObject *parent);
	/// Copy constructor.
	AMSelectableElement(const AMSelectableElement &original);
	/// Null constructor.
	AMSelectableElement();
};

#endif // AMSELECTABLEELEMENT_H
