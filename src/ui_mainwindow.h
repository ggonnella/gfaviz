/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VizMainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionClose;
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *VizMainWindow)
    {
        if (VizMainWindow->objectName().isEmpty())
            VizMainWindow->setObjectName(QStringLiteral("VizMainWindow"));
        VizMainWindow->resize(1281, 870);
        VizMainWindow->setDocumentMode(false);
        actionOpen = new QAction(VizMainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave = new QAction(VizMainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionClose = new QAction(VizMainWindow);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        centralwidget = new QWidget(VizMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 1271, 841));
        tabWidget->setDocumentMode(false);
        tabWidget->setTabsClosable(true);
        tabWidget->setMovable(true);
        VizMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(VizMainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1281, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        VizMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(VizMainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        VizMainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionClose);

        retranslateUi(VizMainWindow);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(VizMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *VizMainWindow)
    {
        VizMainWindow->setWindowTitle(QApplication::translate("VizMainWindow", "GfaViz", nullptr));
        actionOpen->setText(QApplication::translate("VizMainWindow", "Open", nullptr));
        actionSave->setText(QApplication::translate("VizMainWindow", "Save", nullptr));
        actionClose->setText(QApplication::translate("VizMainWindow", "Close", nullptr));
        menuFile->setTitle(QApplication::translate("VizMainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VizMainWindow: public Ui_VizMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
