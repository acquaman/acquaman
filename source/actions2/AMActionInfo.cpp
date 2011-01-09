#include "AMActionInfo.h"

AMActionInfo::AMActionInfo(const QString &shortDescription, const QString & longDescription, const QString &iconFileName, QObject *parent)
	: AMDbObject(parent),
	  shortDescription_(shortDescription),
	  longDescription_(longDescription),
	  iconFileName_(iconFileName)
{
	expectedDuration_ = -1;
}

AMActionInfo::AMActionInfo(const AMActionInfo& other)
	: AMDbObject(),
	  shortDescription_(other.shortDescription_),
	  longDescription_(other.longDescription_),
	  iconFileName_(other.iconFileName_)
{
	expectedDuration_ = other.expectedDuration_;
}

//AMDbThumbnail AMActionInfo::thumbnail(int index) const
//{
//	Q_UNUSED(index)

//	if(iconFileName_.isEmpty())
//		return AMDbThumbnail(shortDescription(), longDescription());
//	else
//		return AMDbThumbnail(shortDescription(), longDescription(), QImage(iconFileName_));
//}

