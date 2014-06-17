#include "AMBuildInfo.h"

AMBuildInfo::AMBuildInfo()
{
	commitSHA_ = "Invalid";
	branchName_ = "Invalid";
	tagName_ = "Invalid";
	describeName_ = "Invalid";
	lastCommitAuthor_ = "Invalid";
	commitDate_ = "Invalid";
}

QString AMBuildInfo::commitSHA() const{
	return commitSHA_;
}

QString AMBuildInfo::branchName() const{
	return branchName_;
}

QString AMBuildInfo::tagName() const{
	return tagName_;
}

QString AMBuildInfo::describeName() const{
	return describeName_;
}

QString AMBuildInfo::lastCommitAuthor() const{
	return lastCommitAuthor_;
}

QString AMBuildInfo::commitDate() const{
	return commitDate_;
}

void AMBuildInfo::setCommitSHA(const QString &commitSHA){
	commitSHA_ = commitSHA;
}

void AMBuildInfo::setBranchName(const QString &branchName){
	branchName_ = branchName;
}

void AMBuildInfo::setTagName(const QString &tagName){
	tagName_ = tagName;
}

void AMBuildInfo::setDescribeName(const QString &describeName){
	describeName_ = describeName;
}

void AMBuildInfo::setLastCommitAuthor(const QString &lastCommitAuthor){
	lastCommitAuthor_ = lastCommitAuthor;
}

void AMBuildInfo::setCommitDate(const QString &commitDate){
	commitDate_ = commitDate;
}
