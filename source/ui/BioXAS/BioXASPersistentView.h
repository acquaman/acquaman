#ifndef BIOXASPERSISTENTVIEW_H
#define BIOXASPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>
#include <QGroupBox>

class AMControl;
class BioXASCryostatView;

class BioXASPersistentView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASPersistentView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASPersistentView();

signals:
	/// Notifier that the selected control in the beam status buttons view has changed.
	void beamlineStatusSelectedComponentChanged(AMControl *control);

public slots:
	/// Refreshes the view.
	void refresh();

protected slots:
	/// Updates the cryostat box.
	void updateCryostatBox();

protected:
	/// The cryostat view.
	BioXASCryostatView *cryostatView_;
	/// The cryostat box.
	QGroupBox *cryostatBox_;
};

#endif // BIOXASPERSISTENTVIEW_H
