#include "AMActionInfo3.h"

AMActionInfo3::AMActionInfo3(const QString &shortDescription, const QString & longDescription, const QString &iconFileName, QObject *parent)
	: AMDbObject(parent),
	  longDescription_(longDescription),
	  iconFileName_(iconFileName)
{
	setName(shortDescription);
	expectedDuration_ = -1;
	canCopy_ = true;
}

AMActionInfo3::AMActionInfo3(const AMActionInfo3& other)
	: AMDbObject(),
	  longDescription_(other.longDescription_),
	  iconFileName_(other.iconFileName_)
{
	setName(other.name());
	expectedDuration_ = other.expectedDuration_;
	canCopy_ = other.canCopy();
}

//AMDbThumbnail AMActionInfo3::thumbnail(int index) const
//{
//	Q_UNUSED(index)

//	if(iconFileName_.isEmpty())
//		return AMDbThumbnail(shortDescription(), longDescription());
//	else
//		return AMDbThumbnail(shortDescription(), longDescription(), QImage(iconFileName_));
//}

