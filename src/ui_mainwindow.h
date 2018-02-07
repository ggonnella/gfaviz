/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VizMainWindow
{
public:
    QWidget *centralwidget;
    QGraphicsView *vizCanvas;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *VizMainWindow)
    {
        if (VizMainWindow->objectName().isEmpty())
            VizMainWindow->setObjectName(QStringLiteral("VizMainWindow"));
        VizMainWindow->resize(1281, 870);
        VizMainWindow->setDocumentMode(false);
        centralwidget = new QWidget(VizMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        vizCanvas = new QGraphicsView(centralwidget);
        vizCanvas->setObjectName(QStringLiteral("vizCanvas"));
        vizCanvas->setGeometry(QRect(0, 0, 1281, 801));
        VizMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(VizMainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1281, 25));
        VizMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(VizMainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        VizMainWindow->setStatusBar(statusbar);

        retranslateUi(VizMainWindow);

        QMetaObject::connectSlotsByName(VizMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *VizMainWindow)
    {
        VizMainWindow->setWindowTitle(QApplication::translate("VizMainWindow", "GfaViz", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VizMainWindow: public Ui_VizMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
