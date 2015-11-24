#ifndef BIOXASFILTERFLIPPERFILTER_H
#define BIOXASFILTERFLIPPERFILTER_H

#include <QObject>

#include "util/AMElement.h"

class BioXASFilterFlipperFilter : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperFilter(const QString &elementSymbol, double thickness, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperFilter();

	/// Returns the filter element symbol.
	QString elementSymbol() const { return elementSymbol_; }
	/// Returns the filter thickness.
	double thickness() const { return thickness_; }
	/// Returns the filter name.
	QString name() const { return name_; }

signals:
	/// Notifier that the filter element has changed.
	void elementChanged(const QString *newSymbol);
	/// Notifier that the filter thickness has changed.
	void thicknessChanged(double newThickness);
	/// Notifier that the filter name has changed.
	void nameChanged(const QString &name);

public slots:
	/// Sets the filter element.
	void setElementSymbol(const QString &newElement);
	/// Sets the filter thickness.
	void setThickness(double newThickness);

	/// Updates the filter name with the current element symbol and filter thickness.
	void updateName();

protected slots:
	/// Sets the filter name.
	void setName(const QString &newName);

	/// Returns a generated name for this filter, based on the given element and thickness.
	static QString generateName(const QString elementSymbol, double thickness);

protected:
	/// The filter element.
	QString elementSymbol_;
	/// The filter thickness.
	double thickness_;
	/// The filter name.
	QString name_;
};

#endif // BIOXASFILTERFLIPPERFILTER_H
