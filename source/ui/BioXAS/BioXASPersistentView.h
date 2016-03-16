#ifndef BIOXASPERSISTENTVIEW_H
#define BIOXASPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>
#include <QGroupBox>

class BioXASCryostatView;

class BioXASPersistentView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASPersistentView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASPersistentView();

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
