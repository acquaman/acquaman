#ifndef AMEXPORTEROPTIONXDIFORMAT_H
#define AMEXPORTEROPTIONXDIFORMAT_H

#include "dataman/export/AMExporterOptionGeneralAscii.h"

/// This exporter option goes with the XDI format exporter.  It holds the edge information.
class AMExporterOptionXDIFormat : public AMExporterOptionGeneralAscii
{
	Q_OBJECT

	Q_PROPERTY(QString elementSymbol READ elementSymbol WRITE setElementSymbol)
	Q_PROPERTY(QString elementEdge READ elementEdge WRITE setElementEdge)

public:
	/// Constructor.
	Q_INVOKABLE AMExporterOptionXDIFormat(QObject *parent = 0);
	/// Copy constructor.
	AMExporterOptionXDIFormat(const AMExporterOptionXDIFormat &original);
	/// Destructor.
	virtual ~AMExporterOptionXDIFormat();

	/// Create a new AMExporterOptionXDIFormat as a copy of the current state.
	virtual AMExporterOption *createCopy() const;

	/// Returns the element symbol.
	QString elementSymbol() const { return elementSymbol_; }
	/// Returns the element edge.
	QString elementEdge() const { return elementEdge_; }

public slots:
	/// Sets the element symbol.
	void setElementSymbol(const QString &symbol);
	/// Sets the element edge.
	void setElementEdge(const QString &edge);

protected:
	/// The element symbol.
	QString elementSymbol_;
	/// The element edge.
	QString elementEdge_;
};

#endif // AMEXPORTEROPTIONXDIFORMAT_H
