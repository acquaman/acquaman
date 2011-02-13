#ifndef AMDEFERREDFUNCTIONCALL_H
#define AMDEFERREDFUNCTIONCALL_H

#include <QObject>
#include <QTimer>

class AMDeferredFunctionCall : public QObject {
	Q_OBJECT

public:
	AMDeferredFunctionCall(QObject* parent = 0);

public slots:
	void schedule() {
		if(!scheduled_) {
			QTimer::singleShot(0, this, SLOT(execute()));
			scheduled_ = true;
		}
	}

	void execute() {
		scheduled_ = false;
		emit executed();
	}

signals:
	void executed();

protected:
	bool scheduled_;
};

#endif // AMDEFERREDFUNCTIONCALL_H
