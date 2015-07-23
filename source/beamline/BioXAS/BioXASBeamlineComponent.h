#ifndef BIOXASBEAMLINECOMPONENT_H
#define BIOXASBEAMLINECOMPONENT_H

#include <QObject>

class BioXASBeamlineComponent : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineComponent(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineComponent();

	/// Returns the current connected state.
	virtual bool isConnected() const = 0;
	/// Returns the current (cached) connected state.
	virtual bool connected() const { return connected_; }

	/// Returns the name.
	virtual QString name() const { return name_; }

signals:
	/// Notifier that the current connected state has changed.
	void connectedChanged(bool isConnected);

protected slots:
	/// Sets the current connected state.
	void setConnected(bool isConnected);
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// The current connected state.
	bool connected_;
	/// The name.
	QString name_;

};

#endif // BIOXASBEAMLINECOMPONENT_H
