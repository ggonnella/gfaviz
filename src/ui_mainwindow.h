/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
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
    QAction *actionRender;
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
        VizMainWindow->resize(1281, 812);
        VizMainWindow->setDocumentMode(false);
        actionOpen = new QAction(VizMainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon(QIcon::fromTheme(QStringLiteral("document-open")));
        actionOpen->setIcon(icon);
        actionRender = new QAction(VizMainWindow);
        actionRender->setObjectName(QStringLiteral("actionRender"));
        QIcon icon1(QIcon::fromTheme(QStringLiteral("document-save-as")));
        actionRender->setIcon(icon1);
        actionClose = new QAction(VizMainWindow);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        QIcon icon2(QIcon::fromTheme(QStringLiteral("application-exit")));
        actionClose->setIcon(icon2);
        centralwidget = new QWidget(VizMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 1271, 761));
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
        menuFile->addAction(actionRender);
        menuFile->addAction(actionClose);

        retranslateUi(VizMainWindow);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(VizMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *VizMainWindow)
    {
        VizMainWindow->setWindowTitle(QApplication::translate("VizMainWindow", "GfaViz", nullptr));
        actionOpen->setText(QApplication::translate("VizMainWindow", "Open GFA file", nullptr));
#ifndef QT_NO_SHORTCUT
        actionOpen->setShortcut(QApplication::translate("VizMainWindow", "Ctrl+O", nullptr));
#endif // QT_NO_SHORTCUT
        actionRender->setText(QApplication::translate("VizMainWindow", "Render to image file", nullptr));
#ifndef QT_NO_SHORTCUT
        actionRender->setShortcut(QApplication::translate("VizMainWindow", "Ctrl+S", nullptr));
#endif // QT_NO_SHORTCUT
        actionClose->setText(QApplication::translate("VizMainWindow", "Exit application", nullptr));
        menuFile->setTitle(QApplication::translate("VizMainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VizMainWindow: public Ui_VizMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
