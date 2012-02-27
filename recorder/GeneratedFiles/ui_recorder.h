/********************************************************************************
** Form generated from reading UI file 'recorder.ui'
**
** Created: Mon 13. Feb 15:07:55 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECORDER_H
#define UI_RECORDER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RecorderClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RecorderClass)
    {
        if (RecorderClass->objectName().isEmpty())
            RecorderClass->setObjectName(QString::fromUtf8("RecorderClass"));
        RecorderClass->resize(600, 400);
        menuBar = new QMenuBar(RecorderClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        RecorderClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RecorderClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        RecorderClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(RecorderClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        RecorderClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(RecorderClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        RecorderClass->setStatusBar(statusBar);

        retranslateUi(RecorderClass);

        QMetaObject::connectSlotsByName(RecorderClass);
    } // setupUi

    void retranslateUi(QMainWindow *RecorderClass)
    {
        RecorderClass->setWindowTitle(QApplication::translate("RecorderClass", "Recorder", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RecorderClass: public Ui_RecorderClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECORDER_H
