#ifndef BIOXASSHUTTERSVIEW_H
#define BIOXASSHUTTERSVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASShutters;
class BioXASControlEditor;

class BioXASShuttersView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASShuttersView(BioXASShutters *shutters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASShuttersView();

	/// Returns the shutters being viewed.
	BioXASShutters* shutters() const { return shutters_; }

signals:
	/// Notifier that the shutters being viewed have changed.
	void shuttersChanged(BioXASShutters *newShutters);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the shutters.
	void setShutters(BioXASShutters *newShutters);

protected:
	/// The shutters being viewed.
	BioXASShutters *shutters_;

	/// The layout.
	QVBoxLayout *layout_;
	/// The list of editors.
	QList<BioXASControlEditor*> editors_;
};

#endif // BIOXASSHUTTERSVIEW_H
