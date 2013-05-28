#include "AMBeamConfigurationView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

#include "ui/AMBeamModel.h"
#include <QVector>
#include <QVector3D>
#include "dataman/database/AMDbObjectSupport.h"

#include <QDebug>


AMBeamConfigurationView::AMBeamConfigurationView(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Beam configuration");

    // GUI SETUP
    QVBoxLayout *vbl = new QVBoxLayout();
    vbl->setSpacing(0);
    vbl->setContentsMargins(0,0,0,0);

    QFrame* oneFrame = new QFrame();
    QHBoxLayout *ohl = new QHBoxLayout();
    ohl->setContentsMargins(12,4,12,4);

    QFrame* twoFrame = new QFrame();
    QHBoxLayout *thl = new QHBoxLayout();
    thl->setContentsMargins(12,4,12,4);

    QFrame* oneOneFrame = new QFrame();
    QVBoxLayout *oovl = new QVBoxLayout();
    oovl->setContentsMargins(12,4,12,4);

    QFrame* oneTwoFrame = new QFrame();
    QVBoxLayout *otvl = new QVBoxLayout();
    otvl->setContentsMargins(12,4,12,4);

    QFrame* oneButtonFrame = new QFrame();
    QVBoxLayout *obvl = new QVBoxLayout();
    obvl->setContentsMargins(12,4,12,4);
    obvl->addStretch();
    obvl->addWidget(oneSelect_ = new QPushButton("Select"));
    obvl->addStretch();
    oneButtonFrame->setLayout(obvl);

    QFrame* twoOneFrame = new QFrame();
    QVBoxLayout *tovl = new QVBoxLayout();
    tovl->setContentsMargins(12,4,12,4);

    QFrame* twoTwoFrame = new QFrame();
    QVBoxLayout *ttvl = new QVBoxLayout();
    ttvl->setContentsMargins(12,4,12,4);


    QFrame* twoButtonFrame = new QFrame();
    QVBoxLayout *tbvl = new QVBoxLayout();
    tbvl->setContentsMargins(12,4,12,4);
    tbvl->addStretch();
    tbvl->addWidget(twoSelect_ = new QPushButton("Select"));
    tbvl->addStretch();
    twoButtonFrame->setLayout(tbvl);

    QFrame* oneOneDataFrame = new QFrame();
    QHBoxLayout *oodhl = new QHBoxLayout();
    oodhl->setContentsMargins(12,4,12,4);
    oodhl->addWidget(new QLabel("One  "));
    oodhl->addSpacing(20);

    QFrame* oneTwoDataFrame = new QFrame();
    QHBoxLayout *otdhl = new QHBoxLayout();
    otdhl->setContentsMargins(12,4,12,4);
    otdhl->addWidget(new QLabel("Two  "));
    otdhl->addSpacing(20);

    QFrame* oneThreeDataFrame = new QFrame();
    QHBoxLayout *oThdhl = new QHBoxLayout();
    oThdhl->setContentsMargins(12,4,12,4);
    oThdhl->addWidget(new QLabel("Three"));
    oThdhl->addSpacing(20);

    QFrame* oneFourDataFrame = new QFrame();
    QHBoxLayout *ofdhl = new QHBoxLayout();
    ofdhl->setContentsMargins(12,4,12,4);
    ofdhl->addWidget(new QLabel("Four "));
    ofdhl->addSpacing(20);

    QFrame* twoOneDataFrame = new QFrame();
    QHBoxLayout *todhl = new QHBoxLayout();
    todhl->setContentsMargins(12,4,12,4);
    todhl->addWidget(new QLabel("One  "));
    todhl->addSpacing(20);

    QFrame* twoTwoDataFrame = new QFrame();
    QHBoxLayout *ttdhl = new QHBoxLayout();
    ttdhl->setContentsMargins(12,4,12,4);
    ttdhl->addWidget(new QLabel("Two  "));
    ttdhl->addSpacing(20);

    QFrame* twoThreeDataFrame = new QFrame();
    QHBoxLayout *tThdhl = new QHBoxLayout();
    tThdhl->setContentsMargins(12,4,12,4);
    tThdhl->addWidget(new QLabel("Three"));
    tThdhl->addSpacing(20);

    QFrame* twoFourDataFrame = new QFrame();
    QHBoxLayout *tfdhl = new QHBoxLayout();
    tfdhl->setContentsMargins(12,4,12,4);
    tfdhl->addWidget(new QLabel("Four "));
    tfdhl->addSpacing(20);

    QFrame* oneOneLabelsFrame = new QFrame();
    QVBoxLayout *oolvl = new QVBoxLayout();
    oolvl->setContentsMargins(12,4,12,4);
    oolvl->addWidget(oneOneX_ = new QLabel("0"));
    oolvl->addWidget(oneOneY_ = new QLabel("0"));
    oolvl->addWidget(oneOneZ_ = new QLabel("0"));
    oolvl->addStretch();
    oneOneLabelsFrame->setLayout(oolvl);

    QFrame* oneTwoLabelsFrame = new QFrame();
    QVBoxLayout *otlvl = new QVBoxLayout();
    otlvl->setContentsMargins(12,4,12,4);
    otlvl->addWidget(oneTwoX_ = new QLabel("0"));
    otlvl->addWidget(oneTwoY_ = new QLabel("0"));
    otlvl->addWidget(oneTwoZ_ = new QLabel("0"));
    otlvl->addStretch();
    oneTwoLabelsFrame->setLayout(otlvl);

    QFrame* oneThreeLabelsFrame = new QFrame();
    QVBoxLayout *oThlvl = new QVBoxLayout();
    oThlvl->setContentsMargins(12,4,12,4);
    oThlvl->addWidget(oneThreeX_ = new QLabel("0"));
    oThlvl->addWidget(oneThreeY_ = new QLabel("0"));
    oThlvl->addWidget(oneThreeZ_ = new QLabel("0"));
    oThlvl->addStretch();
    oneThreeLabelsFrame->setLayout(oThlvl);


    QFrame* oneFourLabelsFrame = new QFrame();
    QVBoxLayout *oflvl = new QVBoxLayout();
    oflvl->setContentsMargins(12,4,12,4);
    oflvl->addWidget(oneFourX_ = new QLabel("0"));
    oflvl->addWidget(oneFourY_ = new QLabel("0"));
    oflvl->addWidget(oneFourZ_ = new QLabel("0"));
    oflvl->addStretch();
    oneFourLabelsFrame->setLayout(oflvl);

    QFrame* twoOneLabelsFrame = new QFrame();
    QVBoxLayout *tolvl = new QVBoxLayout();
    tolvl->setContentsMargins(12,4,12,4);
    tolvl->addWidget(twoOneX_ = new QLabel("0"));
    tolvl->addWidget(twoOneY_ = new QLabel("0"));
    tolvl->addWidget(twoOneZ_ = new QLabel("0"));
    tolvl->addStretch();
    twoOneLabelsFrame->setLayout(tolvl);

    QFrame* twoTwoLabelsFrame = new QFrame();
    QVBoxLayout *ttlvl = new QVBoxLayout();
    ttlvl->setContentsMargins(12,4,12,4);
    ttlvl->addWidget(twoTwoX_ = new QLabel("0"));
    ttlvl->addWidget(twoTwoY_ = new QLabel("0"));
    ttlvl->addWidget(twoTwoZ_ = new QLabel("0"));
    ttlvl->addStretch();
    twoTwoLabelsFrame->setLayout(ttlvl);

    QFrame* twoThreeLabelsFrame = new QFrame();
    QVBoxLayout *tThlvl = new QVBoxLayout();
    tThlvl->setContentsMargins(12,4,12,4);
    tThlvl->addWidget(twoThreeX_ = new QLabel("0"));
    tThlvl->addWidget(twoThreeY_ = new QLabel("0"));
    tThlvl->addWidget(twoThreeZ_ = new QLabel("0"));
    tThlvl->addStretch();
    twoThreeLabelsFrame->setLayout(tThlvl);


    QFrame* twoFourLabelsFrame = new QFrame();
    QVBoxLayout *tflvl = new QVBoxLayout();
    tflvl->setContentsMargins(12,4,12,4);
    tflvl->addWidget(twoFourX_ = new QLabel("0"));
    tflvl->addWidget(twoFourY_ = new QLabel("0"));
    tflvl->addWidget(twoFourZ_ = new QLabel("0"));
    tflvl->addStretch();
    twoFourLabelsFrame->setLayout(tflvl);

    oodhl->addWidget(oneOneLabelsFrame);
    oodhl->addStretch();
    oneOneDataFrame->setLayout(oodhl);

    otdhl->addWidget(oneTwoLabelsFrame);
    otdhl->addStretch();
    oneTwoDataFrame->setLayout(otdhl);

    oThdhl->addWidget(oneThreeLabelsFrame);
    oThdhl->addStretch();
    oneThreeDataFrame->setLayout(oThdhl);

    ofdhl->addWidget(oneFourLabelsFrame);
    ofdhl->addStretch();
    oneFourDataFrame->setLayout(ofdhl);

    todhl->addWidget(twoOneLabelsFrame);
    todhl->addStretch();
    twoOneDataFrame->setLayout(todhl);

    ttdhl->addWidget(twoTwoLabelsFrame);
    ttdhl->addStretch();
    twoTwoDataFrame->setLayout(ttdhl);

    tThdhl->addWidget(twoThreeLabelsFrame);
    tThdhl->addStretch();
    twoThreeDataFrame->setLayout(tThdhl);

    tfdhl->addWidget(twoFourLabelsFrame);
    tfdhl->addStretch();
    twoFourDataFrame->setLayout(tfdhl);

    oovl->addWidget(oneOneDataFrame);
    oovl->addWidget(oneFourDataFrame);
    oovl->addStretch();
    oneOneFrame->setLayout(oovl);

    otvl->addWidget(oneTwoDataFrame);
    otvl->addWidget(oneThreeDataFrame);
    otvl->addStretch();
    oneTwoFrame->setLayout(otvl);

    tovl->addWidget(twoOneDataFrame);
    tovl->addWidget(twoFourDataFrame);
    tovl->addStretch();
    twoOneFrame->setLayout(tovl);

    ttvl->addWidget(twoTwoDataFrame);
    ttvl->addWidget(twoThreeDataFrame);
    ttvl->addStretch();
    twoTwoFrame->setLayout(ttvl);

    ohl->addWidget(oneOneFrame);
    ohl->addWidget(oneTwoFrame);
    ohl->addWidget(oneButtonFrame);
    oneFrame->setLayout(ohl);

    thl->addWidget(twoOneFrame);
    thl->addWidget(twoTwoFrame);
    thl->addWidget(twoButtonFrame);
    twoFrame->setLayout(thl);

    vbl->addWidget(oneFrame);
    vbl->addWidget(twoFrame);
    vbl->addStretch();
    vbl->addWidget(intersectionButton_ = new QPushButton);
    vbl->addStretch();
    setLayout(vbl);




    // make connections

    connect(oneSelect_, SIGNAL(clicked()), this, SIGNAL(oneSelect()));
    connect(twoSelect_, SIGNAL(clicked()), this, SIGNAL(twoSelect()));
    connect(intersectionButton_, SIGNAL(clicked()), this, SIGNAL(intersection()));



}

void AMBeamConfigurationView::beamChanged(QObject *newBeam)
{
    qDebug()<<"changing beam";
    AMBeamConfiguration *beam = qobject_cast<AMBeamConfiguration*>(newBeam);
    if(beam != 0)
    {
        beamModel_ = beam;
        updateData();
    }
    else
    {
        qDebug()<<"Cannot assign null to beamModel...(AMBeamConfiguration)";
    }
}


void AMBeamConfigurationView::updateData()
{
    qDebug()<<"Updating labels";
    QVector<QVector3D> positionOne = beamModel_->positionOne();
    QVector<QVector3D> positionTwo = beamModel_->positionTwo();
    for(int i = 0; i < 4; i++)
    {
        if(!positionOne.isEmpty() && !positionOne[i].isNull())
        {

            switch(i)
            {
                case 0: updateOneOne(positionOne[i]);
                        break;
                case 1: updateOneTwo(positionOne[i]);
                        break;
                case 2: updateOneThree(positionOne[i]);
                        break;
                case 3: updateOneFour(positionOne[i]);
                        break;
            }


        }
        else
        {
            qDebug()<<"invalid first shape";
        }

        if(!positionTwo.isEmpty() && !positionTwo[i].isNull())
        {
            switch(i)
            {
                case 0: updateTwoOne(positionTwo[i]);
                        break;
                case 1: updateTwoTwo(positionTwo[i]);
                        break;
                case 2: updateTwoThree(positionTwo[i]);
                        break;
                case 3: updateTwoFour(positionTwo[i]);
                        break;
            }

        }
        else
        {
            qDebug()<<"invalid second shape";
        }
    }




}

void AMBeamConfigurationView::updateOneOne(QVector3D one)
{
    QString x = QString::number(one.x());
    QString y = QString::number(one.y());
    QString z = QString::number(one.z());

    oneOneX_->setText(x);
    oneOneY_->setText(y);
    oneOneZ_->setText(z);
}

void AMBeamConfigurationView::updateOneTwo(QVector3D two)
{
    QString x = QString::number(two.x());
    QString y = QString::number(two.y());
    QString z = QString::number(two.z());

    oneTwoX_->setText(x);
    oneTwoY_->setText(y);
    oneTwoZ_->setText(z);
}

void AMBeamConfigurationView::updateOneThree(QVector3D three)
{
    QString x = QString::number(three.x());
    QString y = QString::number(three.y());
    QString z = QString::number(three.z());

    oneThreeX_->setText(x);
    oneThreeY_->setText(y);
    oneThreeZ_->setText(z);
}

void AMBeamConfigurationView::updateOneFour(QVector3D four)
{
    QString x = QString::number(four.x());
    QString y = QString::number(four.y());
    QString z = QString::number(four.z());

    oneFourX_->setText(x);
    oneFourY_->setText(y);
    oneFourZ_->setText(z);
}

void AMBeamConfigurationView::updateTwoOne(QVector3D one)
{
    QString x = QString::number(one.x());
    QString y = QString::number(one.y());
    QString z = QString::number(one.z());

    twoOneX_->setText(x);
    twoOneY_->setText(y);
    twoOneZ_->setText(z);
}

void AMBeamConfigurationView::updateTwoTwo(QVector3D two)
{
    QString x = QString::number(two.x());
    QString y = QString::number(two.y());
    QString z = QString::number(two.z());

    twoTwoX_->setText(x);
    twoTwoY_->setText(y);
    twoTwoZ_->setText(z);
}

void AMBeamConfigurationView::updateTwoThree(QVector3D three)
{
    QString x = QString::number(three.x());
    QString y = QString::number(three.y());
    QString z = QString::number(three.z());

    twoThreeX_->setText(x);
    twoThreeY_->setText(y);
    twoThreeZ_->setText(z);
}

void AMBeamConfigurationView::updateTwoFour(QVector3D four)
{
    QString x = QString::number(four.x());
    QString y = QString::number(four.y());
    QString z = QString::number(four.z());

    twoFourX_->setText(x);
    twoFourY_->setText(y);
    twoFourZ_->setText(z);
}
