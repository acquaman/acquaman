#ifndef AMBEAMCONFIGURATION_H
#define AMBEAMCONFIGURATION_H

#include <QWidget>
#include <QVector3D>

class QLabel;
class QPushButton;
class AMBeamConfiguration;
class QVector3D;

class AMBeamConfigurationView : public QWidget
{
    Q_OBJECT
public:
    AMBeamConfigurationView(AMBeamConfiguration *beam, QWidget *parent = 0);

public slots:
    void beamChanged(QObject*);

signals:
    void oneSelect();
    void twoSelect();
    void intersection();

protected:
    QLabel *oneOneX_;
    QLabel *oneOneY_;
    QLabel *oneOneZ_;

    QLabel *oneTwoX_;
    QLabel *oneTwoY_;
    QLabel *oneTwoZ_;

    QLabel *oneThreeX_;
    QLabel *oneThreeY_;
    QLabel *oneThreeZ_;

    QLabel *oneFourX_;
    QLabel *oneFourY_;
    QLabel *oneFourZ_;

    QLabel *twoOneX_;
    QLabel *twoOneY_;
    QLabel *twoOneZ_;

    QLabel *twoTwoX_;
    QLabel *twoTwoY_;
    QLabel *twoTwoZ_;

    QLabel *twoThreeX_;
    QLabel *twoThreeY_;
    QLabel *twoThreeZ_;

    QLabel *twoFourX_;
    QLabel *twoFourY_;
    QLabel *twoFourZ_;

    QPushButton *oneSelect_;
    QPushButton *twoSelect_;

    QPushButton *intersectionButton_;

    AMBeamConfiguration* beamModel_;

protected slots:
    void updateData();
    void updateOneOne(QVector3D);
    void updateOneTwo(QVector3D);
    void updateOneThree(QVector3D);
    void updateOneFour(QVector3D);
    void updateTwoOne(QVector3D);
    void updateTwoTwo(QVector3D);
    void updateTwoThree(QVector3D);
    void updateTwoFour(QVector3D);
};

#endif // AMBEAMCONFIGURATION_H
