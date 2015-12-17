#ifndef BIOXASSSRLMONOCHROMATORVIEW_H
#define BIOXASSSRLMONOCHROMATORVIEW_H

#include <QWidget>
#include <QLayout>

class AMControlStopButton;
class BioXASSSRLMonochromatorConfigurationView;
class BioXASSSRLMonochromator;

class BioXASSSRLMonochromatorView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorView();

	/// Returns the mono being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the mono being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newMono);

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// The mono stop button.
	AMControlStopButton *stopButton_;
	/// The mono configuration view.
	BioXASSSRLMonochromatorConfigurationView *configurationView_;
};

#endif // BIOXASSSRLMONOCHROMATORVIEW_H
