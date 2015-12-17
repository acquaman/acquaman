#ifndef BIOXASPERSISTENTVIEW_H
#define BIOXASPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>
#include <QGroupBox>

class BioXASPersistentView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASPersistentView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASPersistentView();
};

#endif // BIOXASPERSISTENTVIEW_H
