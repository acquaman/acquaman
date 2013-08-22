#ifndef AMSAMPLEPLATEBROWSERVIEW_H
#define AMSAMPLEPLATEBROWSERVIEW_H

#include <QWidget>
#include <QDialog>

class QListView;
class QLineEdit;
class QPushButton;

class AMSamplePlate;
class AMSamplePlateBrowser;

class AMSamplePlateBrowserView : public QWidget
{
Q_OBJECT
public:
	AMSamplePlateBrowserView(AMSamplePlateBrowser *samplePlateBrowser, QWidget *parent = 0);

signals:
	void samplePlateSelected(AMSamplePlate *samplePlate);

protected slots:
	void onSelectButtonClicked();

protected:
	AMSamplePlateBrowser *samplePlateBrowser_;
	QListView *samplePlatesListView_;
	QPushButton *selectButton_;
	QPushButton *cancelButton_;
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
