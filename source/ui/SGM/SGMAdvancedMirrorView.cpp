#include "SGMAdvancedMirrorView.h"

#include <QBoxLayout>
#include <QDebug>

SGMAdvancedMirrorView::SGMAdvancedMirrorView(QWidget *parent) :
	QWidget(parent)
{
	qDebug() << "Start of mirror constructor";

	testLabel_ = new QLabel("Test");
	testLabel2_ = new QLabel("bob");
	testLabel3_ = new QLabel("jane");
	testLabel4_ = new QLabel("tom");
	testLabel5_ = new QLabel("pen");
	testLabel6_ = new QLabel("note");
	testLabel7_ = new QLabel("jane2");
	testLabel8_ = new QLabel("tom2");
	testLabel9_ = new QLabel("pen2");
	testLabel10_ = new QLabel("note2");
	testLabel11_ = new QLabel("jane3");
	testLabel12_ = new QLabel("tom3");
	testLabel13_ = new QLabel("pen3");
	testLabel14_ = new QLabel("note3");
	testLabel15_ = new QLabel("jane4");
	testLabel16_ = new QLabel("tom4");
	testLabel17_ = new QLabel("pen4");
	testLabel18_ = new QLabel("note4");
	testLabel19_ = new QLabel("jane5");
	testLabel20_ = new QLabel("tom5");
	testLabel21_ = new QLabel("pen5");
	testLabel22_ = new QLabel("note5");
	testLabel23_ = new QLabel("jane6");
	testLabel24_ = new QLabel("tom6");
	testLabel25_ = new QLabel("pen6");
	testLabel26_ = new QLabel("note6");
	testLabel27_ = new QLabel("jane7");
	testLabel28_ = new QLabel("tom7");
	testLabel29_ = new QLabel("pen7");
	testLabel30_ = new QLabel("note7");
	testLabel31_ = new QLabel("jane8");
	testLabel32_ = new QLabel("tom8");
	testLabel33_ = new QLabel("pen8");
	testLabel34_ = new QLabel("note8");
	testLabel35_ = new QLabel("jane9");
	testLabel36_ = new QLabel("tom9");
	testLabel37_ = new QLabel("pen9");
	testLabel38_ = new QLabel("note9");
	testLabel39_ = new QLabel("jane10");
	testLabel40_ = new QLabel("tom10");
	testLabel41_ = new QLabel("pen10");
	testLabel42_ = new QLabel("note10");
	QVBoxLayout *verticalLayoutLeft = new QVBoxLayout();
	verticalLayoutLeft->addWidget(testLabel_);

	QHBoxLayout *horizontalLayout2 = new QHBoxLayout();
	horizontalLayout2->addWidget(testLabel3_);
	verticalLayoutLeft->addLayout(horizontalLayout2);

	QVBoxLayout *verticalLayoutHL21 = new QVBoxLayout();
	verticalLayoutHL21->addWidget(testLabel4_);
	horizontalLayout2->addLayout(verticalLayoutHL21);

	QVBoxLayout *verticalLayoutHL22 = new QVBoxLayout();
	verticalLayoutHL22->addWidget(testLabel5_);
	horizontalLayout2->addLayout(verticalLayoutHL22);

	QVBoxLayout *verticalLayoutHL23 = new QVBoxLayout();
	verticalLayoutHL23->addWidget(testLabel6_);
	horizontalLayout2->addLayout(verticalLayoutHL23);

	QHBoxLayout *horizontalLayout3 = new QHBoxLayout();
	horizontalLayout3->addWidget(testLabel7_);
	verticalLayoutLeft->addLayout(horizontalLayout3);

	QVBoxLayout *verticalLayoutHL31 = new QVBoxLayout();
	verticalLayoutHL31->addWidget(testLabel8_);
	horizontalLayout3->addLayout(verticalLayoutHL31);

	QVBoxLayout *verticalLayoutHL32 = new QVBoxLayout();
	verticalLayoutHL32->addWidget(testLabel9_);
	horizontalLayout3->addLayout(verticalLayoutHL32);

	QVBoxLayout *verticalLayoutHL33 = new QVBoxLayout();
	verticalLayoutHL33->addWidget(testLabel10_);
	horizontalLayout3->addLayout(verticalLayoutHL33);

	QHBoxLayout *horizontalLayout4 = new QHBoxLayout();
	horizontalLayout4->addWidget(testLabel11_);
	verticalLayoutLeft->addLayout(horizontalLayout4);

	QVBoxLayout *verticalLayoutHL41 = new QVBoxLayout();
	verticalLayoutHL41->addWidget(testLabel12_);
	horizontalLayout4->addLayout(verticalLayoutHL41);

	QVBoxLayout *verticalLayoutHL42 = new QVBoxLayout();
	verticalLayoutHL42->addWidget(testLabel13_);
	horizontalLayout4->addLayout(verticalLayoutHL42);

	QVBoxLayout *verticalLayoutHL43 = new QVBoxLayout();
	verticalLayoutHL43->addWidget(testLabel14_);
	horizontalLayout4->addLayout(verticalLayoutHL43);

	QHBoxLayout *horizontalLayout5 = new QHBoxLayout();
	horizontalLayout5->addWidget(testLabel15_);
	verticalLayoutLeft->addLayout(horizontalLayout5);

	QVBoxLayout *verticalLayoutHL51 = new QVBoxLayout();
	verticalLayoutHL51->addWidget(testLabel16_);
	horizontalLayout5->addLayout(verticalLayoutHL51);

	QVBoxLayout *verticalLayoutHL52 = new QVBoxLayout();
	verticalLayoutHL52->addWidget(testLabel17_);
	horizontalLayout5->addLayout(verticalLayoutHL52);

	QVBoxLayout *verticalLayoutHL53 = new QVBoxLayout();
	verticalLayoutHL53->addWidget(testLabel18_);
	horizontalLayout5->addLayout(verticalLayoutHL53);

	QHBoxLayout *horizontalLayout6 = new QHBoxLayout();
	horizontalLayout6->addWidget(testLabel19_);
	verticalLayoutLeft->addLayout(horizontalLayout6);

	QVBoxLayout *verticalLayoutHL61 = new QVBoxLayout();
	verticalLayoutHL61->addWidget(testLabel20_);
	horizontalLayout6->addLayout(verticalLayoutHL61);

	QVBoxLayout *verticalLayoutHL62 = new QVBoxLayout();
	verticalLayoutHL62->addWidget(testLabel21_);
	horizontalLayout6->addLayout(verticalLayoutHL62);

	QVBoxLayout *verticalLayoutHL63 = new QVBoxLayout();
	verticalLayoutHL63->addWidget(testLabel22_);
	horizontalLayout6->addLayout(verticalLayoutHL63);




	QVBoxLayout *verticalLayoutRight = new QVBoxLayout();
	verticalLayoutRight->addWidget(testLabel2_);

	QHBoxLayout *horizontalLayout7 = new QHBoxLayout();
	horizontalLayout7->addWidget(testLabel23_);
	verticalLayoutRight->addLayout(horizontalLayout7);

	QVBoxLayout *verticalLayoutHL71 = new QVBoxLayout();
	verticalLayoutHL71->addWidget(testLabel24_);
	horizontalLayout7->addLayout(verticalLayoutHL71);

	QVBoxLayout *verticalLayoutHL72 = new QVBoxLayout();
	verticalLayoutHL72->addWidget(testLabel25_);
	horizontalLayout7->addLayout(verticalLayoutHL72);

	QVBoxLayout *verticalLayoutHL73 = new QVBoxLayout();
	verticalLayoutHL73->addWidget(testLabel26_);
	horizontalLayout7->addLayout(verticalLayoutHL73);

	QHBoxLayout *horizontalLayout8 = new QHBoxLayout();
	horizontalLayout8->addWidget(testLabel27_);
	verticalLayoutRight->addLayout(horizontalLayout8);

	QVBoxLayout *verticalLayoutHL81 = new QVBoxLayout();
	verticalLayoutHL81->addWidget(testLabel28_);
	horizontalLayout8->addLayout(verticalLayoutHL81);

	QVBoxLayout *verticalLayoutHL82 = new QVBoxLayout();
	verticalLayoutHL82->addWidget(testLabel29_);
	horizontalLayout8->addLayout(verticalLayoutHL82);

	QVBoxLayout *verticalLayoutHL83 = new QVBoxLayout();
	verticalLayoutHL83->addWidget(testLabel30_);
	horizontalLayout8->addLayout(verticalLayoutHL83);

	QHBoxLayout *horizontalLayout9 = new QHBoxLayout();
	horizontalLayout9->addWidget(testLabel31_);
	verticalLayoutRight->addLayout(horizontalLayout9);

	QVBoxLayout *verticalLayoutHL91 = new QVBoxLayout();
	verticalLayoutHL91->addWidget(testLabel32_);
	horizontalLayout9 ->addLayout(verticalLayoutHL91);

	QVBoxLayout *verticalLayoutHL92 = new QVBoxLayout();
	verticalLayoutHL92->addWidget(testLabel33_);
	horizontalLayout9->addLayout(verticalLayoutHL92);

	QVBoxLayout *verticalLayoutHL93 = new QVBoxLayout();
	verticalLayoutHL93->addWidget(testLabel34_);
	horizontalLayout9->addLayout(verticalLayoutHL93);

	QHBoxLayout *horizontalLayout10 = new QHBoxLayout();
	horizontalLayout10->addWidget(testLabel35_);
	verticalLayoutRight->addLayout(horizontalLayout10);

	QVBoxLayout *verticalLayoutHL101 = new QVBoxLayout();
	verticalLayoutHL101->addWidget(testLabel36_);
	horizontalLayout10->addLayout(verticalLayoutHL101);

	QVBoxLayout *verticalLayoutHL102 = new QVBoxLayout();
	verticalLayoutHL102->addWidget(testLabel37_);
	horizontalLayout10->addLayout(verticalLayoutHL102);

	QVBoxLayout *verticalLayoutHL103 = new QVBoxLayout();
	verticalLayoutHL103->addWidget(testLabel38_);
	horizontalLayout10->addLayout(verticalLayoutHL103);

	QHBoxLayout *horizontalLayout11 = new QHBoxLayout();
	horizontalLayout11->addWidget(testLabel39_);
	verticalLayoutRight->addLayout(horizontalLayout11);

	QVBoxLayout *verticalLayoutHL111 = new QVBoxLayout();
	verticalLayoutHL111->addWidget(testLabel40_);
	horizontalLayout11->addLayout(verticalLayoutHL111);

	QVBoxLayout *verticalLayoutHL112 = new QVBoxLayout();
	verticalLayoutHL112->addWidget(testLabel41_);
	horizontalLayout11->addLayout(verticalLayoutHL112);

	QVBoxLayout *verticalLayoutHL113 = new QVBoxLayout();
	verticalLayoutHL113->addWidget(testLabel42_);
	horizontalLayout11->addLayout(verticalLayoutHL113);










	QHBoxLayout *horizontalLayout = new QHBoxLayout();
	horizontalLayout->addLayout(verticalLayoutLeft);
	horizontalLayout->addLayout(verticalLayoutRight);
	setLayout(horizontalLayout);

	qDebug() << "End of mirror constructor";
}
