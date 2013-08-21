#ifndef AMSAMPLEPLATEBROWSERVIEW_H
#define AMSAMPLEPLATEBROWSERVIEW_H

#include <QWidget>
#include <QDialog>

class QListView;
class QLineEdit;
class QPushButton;

class AMSamplePlateBrowser;

class AMSamplePlateBrowserView : public QWidget
{
public:
	AMSamplePlateBrowserView(AMSamplePlateBrowser *samplePlateBrowser, QWidget *parent = 0);

protected:
	AMSamplePlateBrowser *samplePlateBrowser_;
	QListView *samplePlatesListView_;
};

class AMSamplePlateCreationDialog : public QDialog
{
Q_OBJECT
public:
	AMSamplePlateCreationDialog(QWidget *parent = 0);

	QString samplePlateName() const;

protected slots:
	void onSamplePlateNameLineEditTextChanged(const QString &name);

protected:
	QLineEdit *samplePlateNameLineEdit_;
	QPushButton *okButton_;
	QPushButton *cancelButton_;
};

#endif // AMSAMPLEPLATEBROWSERVIEW_H
