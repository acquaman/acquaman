#ifndef VESPERSSETSTRINGACTIONINFO_H
#define VESPERSSETSTRINGACTIONINFO_H

#include "actions3/AMActionInfo3.h"

/// This is a VESPERS specific action info used for setting PV's that are ascii arrays instead of strings.
class VESPERSSetStringActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY (QString text READ text WRITE setText)

public:
	/// Constructor.  Requires the text that will be set.
	VESPERSSetStringActionInfo(const QString &newText, QObject *parent = 0);
	/// Copy constructor.
	VESPERSSetStringActionInfo(const VESPERSSetStringActionInfo &other);
	/// Destructor.
	virtual ~VESPERSSetStringActionInfo();

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const { return new VESPERSSetStringActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "VESPERS specific ascii setter."; }

	/// Returns the text that will be set.
	const QString &text() const { return text_; }
	/// Sets the new text.
	void setText(const QString &newText);

protected:
	/// The text for the string.
	QString text_;
};

#endif // VESPERSSETSTRINGACTIONINFO_H
