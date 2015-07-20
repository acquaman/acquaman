#-------------------------------------------------
#
# Project created by QtCreator 2015-06-02T22:25:47
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AMGithubProjectManager
TEMPLATE = app

include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )
include ( $$PATH_TO_AM/compositeCommon/AMAction.pri )
include ( $$PATH_TO_AM/compositeCommon/QJSON.pri )
include ( $$PATH_TO_AM/compositeCommon/AMMPlot.pri )

SOURCES += \
	AMGithubProjectManager/main.cpp \
	AMGithubProjectManager/AMGithubProjectManagerMainView.cpp \
	source/actions3/actions/AMRestActionInfo.cpp \
	source/actions3/actions/AMRestAction.cpp \
    AMGithubProjectManager/AMGitHubIssue.cpp \
    AMGithubProjectManager/AMGitHubIssueFamily.cpp \
    AMGithubProjectManager/AMGitHubIssueFamilyView.cpp \
    AMGithubProjectManager/AMGitHubComplexityManager.cpp \
    AMGithubProjectManager/AMGitHubProject.cpp \
    AMGithubProjectManager/AMGitHubMilestone.cpp \
    source/actions3/actions/AMGitHubGetIssuesActionInfo.cpp \
    source/actions3/actions/AMGitHubGetIssuesAction.cpp

HEADERS  += \
	AMGithubProjectManager/AMGithubProjectManagerMainView.h \
	source/actions3/actions/AMRestActionInfo.h \
	source/actions3/actions/AMRestAction.h \
    AMGithubProjectManager/AMGitHubIssue.h \
    AMGithubProjectManager/AMGitHubIssueFamily.h \
    AMGithubProjectManager/AMGitHubIssueFamilyView.h \
    AMGithubProjectManager/AMGitHubComplexityManager.h \
    AMGithubProjectManager/AMGitHubProject.h \
    AMGithubProjectManager/AMGitHubMilestone.h \
    source/actions3/actions/AMGitHubGetIssuesActionInfo.h \
    source/actions3/actions/AMGitHubGetIssuesAction.h
