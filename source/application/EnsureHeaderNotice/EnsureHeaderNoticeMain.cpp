#include <QtCore/QCoreApplication>

#include "HeaderNoticeChecker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList arguments = a.arguments();
    if(arguments.count() < 2)
	    return -1;

    QString oldNotice = "Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.";
    QString newNotice = "Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.\nCopyright 2013-2014 David Chevrier and Darren Hunter.";
    HeaderNoticeChecker *checker = new HeaderNoticeChecker(oldNotice, newNotice, arguments.at(1));

    Q_UNUSED(checker)

    return a.exec();
}
