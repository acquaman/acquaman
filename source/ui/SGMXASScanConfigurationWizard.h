#ifndef SGMXASSCANCONFIGURATIONWIZARD_H
#define SGMXASSCANCONFIGURATIONWIZARD_H

#include <QWizard>
#include <QWizardPage>
#include "AMControlSetView.h"
#include "AMDetectorView.h"
#include "AMXASRegionsView.h"
#include "AMRegionsLineView.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"

class SGMXASScanConfigurationIntroWizardPage;
class AMXASRegionsWizardPage;
class SGMFluxResolutionWizardPage;
class AMControlSetWizardPage;
class AMDetectorSetWizardPage;
class SGMXASScanConfigurationReviewWizardPage;

class SGMXASScanConfigurationWizard : public QWizard
{
Q_OBJECT
public:
	explicit SGMXASScanConfigurationWizard(QWidget *parent = 0);

	void accept();

protected:
	SGMXASScanConfiguration *cfg_;
	AMDetectorInfoSet *cfgDetectorInfoSet_;

	SGMXASScanConfigurationIntroWizardPage *introPage;
	AMXASRegionsWizardPage *regionsPage;
	SGMFluxResolutionWizardPage *fluxResolutionPage;
	AMControlSetWizardPage *trackingPage;
	AMDetectorSetWizardPage *detectorsPage;
	SGMXASScanConfigurationReviewWizardPage *reviewPage;
};

class SGMXASScanConfigurationIntroWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	SGMXASScanConfigurationIntroWizardPage(QString title, QString subTitle, QWidget *parent = 0);

protected slots:
	void resizeEvent(QResizeEvent *);

protected:
	QGridLayout *gl_;
	QLabel *textLabel_;
};

class AMXASRegionsWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	AMXASRegionsWizardPage(AMXASRegionsList *regionsList, QString title, QString subTitle, QWidget *parent = 0);

protected slots:
	void resizeEvent(QResizeEvent *);

protected:
	AMXASRegionsList* regionsList_;

	AMXASRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
	QGridLayout *gl_;
	QLabel *textLabel_;
};

class SGMFluxResolutionWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	SGMFluxResolutionWizardPage(AMControlOptimizationSet *fluxResolutionSet, AMXASRegionsList* regionsList, QString title, QString subTitle, QWidget *parent = 0);

public slots:
	void onRegionsUpdate();

protected slots:
	void resizeEvent(QResizeEvent *);

protected:
	AMControlOptimizationSet *fluxResolutionSet_;
	AMXASRegionsList* regionsList_;

	AMCompactControlOptimizationSetView *fluxResolutionView_;
	QGridLayout *gl_;
	QLabel *textLabel_;
};

class AMControlSetWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	AMControlSetWizardPage(AMControlSet *trackingSet, QString title, QString subTitle, QWidget *parent = 0);

protected slots:
	void resizeEvent(QResizeEvent *);

protected:
	AMControlSet *trackingSet_;

	AMControlSetView *trackingView_;
	QGridLayout *gl_;
	QLabel *textLabel_;
};

class AMDetectorSetWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	AMDetectorSetWizardPage(AMDetectorInfoSet *detectorSet, AMDetectorInfoSet *cfgDetectorInfoSet, QString title, QString subTitle, QWidget *parent = 0);

protected slots:
	void resizeEvent(QResizeEvent *);

protected:
	AMDetectorInfoSet *detectorSet_;
	AMDetectorInfoSet *cfgDetectorInfoSet_;

	AMDetectorInfoSetView *detectorView_;
	QGridLayout *gl_;
	QLabel *textLabel_;
};

class SGMXASScanConfigurationReviewWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	SGMXASScanConfigurationReviewWizardPage(QString title, QString subTitle, QWidget *parent = 0);

protected slots:
	void resizeEvent(QResizeEvent *);

protected:
	QGridLayout *gl_;
	QLabel *textLabel_;
};

#endif // SGMXASSCANCONFIGURATIONWIZARD_H
