#ifndef BIOXASFILTERFLIPPERFILTER_H
#define BIOXASFILTERFLIPPERFILTER_H

#include <QObject>

#include "util/AMElement.h"

class BioXASFilterFlipperFilter : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperFilter(AMElement *element, double thickness, QObject *parent = 0, const QString &name = QString());
	/// Destructor.
	virtual ~BioXASFilterFlipperFilter();

	/// Returns the filter element.
	AMElement* element() const { return element_; }
	/// Returns the filter name.
	QString name() const { return name_; }
	/// Returns the filter thickness.
	double thickness() const { return thickness_; }

signals:
	/// Notifier that the filter element has changed.
	void elementChanged(AMElement *newElement);
	/// Notifier that the filter name has changed.
	void nameChanged(const QString &name);
	/// Notifier that the filter thickness has changed.
	void thicknessChanged(double newThickness);

public slots:
	/// Sets the filter element.
	void setElement(AMElement *newElement);
	/// Sets the filter name.
	void setName(const QString &newName);
	/// Sets the filter thickness.
	void setThickness(double newThickness);

protected:
	/// Returns a generated name for this filter, based on the given element and thickness.
	static QString generateName(AMElement *element, double thickness);

protected:
	/// The filter element.
	AMElement *element_;
	/// The filter name.
	QString name_;
	/// The filter thickness.
	double thickness_;
};

#endif // BIOXASFILTERFLIPPERFILTER_H
