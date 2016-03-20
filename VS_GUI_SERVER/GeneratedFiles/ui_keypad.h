/********************************************************************************
** Form generated from reading UI file 'keypad.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYPAD_H
#define UI_KEYPAD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_KeyPad
{
public:
    QLabel *label_2;
    QLabel *label;
    QPushButton *pushButton_KP1;
    QPushButton *pushButton_KP2;
    QPushButton *pushButton_KP3;
    QPushButton *pushButton_KP4;
    QPushButton *pushButton_KP5;
    QPushButton *pushButton_KP6;
    QPushButton *pushButton_KP7;
    QPushButton *pushButton_KP8;
    QPushButton *pushButton_KP9;
    QPushButton *pushButton_KP0;
    QPushButton *pushButton_KP_BS;
    QPushButton *pushButton_KP_CLS;
    QPushButton *ButtonOk;
    QLineEdit *lineEdit_MSG;
    QLineEdit *lineEdit_PIN;

    void setupUi(QDialog *KeyPad)
    {
        if (KeyPad->objectName().isEmpty())
            KeyPad->setObjectName(QString::fromUtf8("KeyPad"));
        KeyPad->resize(800, 570);
        label_2 = new QLabel(KeyPad);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(520, 20, 146, 22));
        QFont font;
        font.setFamily(QString::fromUtf8("Sans Serif"));
        font.setPointSize(15);
        font.setBold(false);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(50);
        font.setStrikeOut(false);
        label_2->setFont(font);
        label = new QLabel(KeyPad);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 280, 22));
        label->setFont(font);
        pushButton_KP1 = new QPushButton(KeyPad);
        pushButton_KP1->setObjectName(QString::fromUtf8("pushButton_KP1"));
        pushButton_KP1->setEnabled(false);
        pushButton_KP1->setGeometry(QRect(10, 130, 148, 130));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Serif"));
        font1.setPointSize(50);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setWeight(50);
        font1.setStrikeOut(false);
        pushButton_KP1->setFont(font1);
        pushButton_KP2 = new QPushButton(KeyPad);
        pushButton_KP2->setObjectName(QString::fromUtf8("pushButton_KP2"));
        pushButton_KP2->setEnabled(false);
        pushButton_KP2->setGeometry(QRect(168, 130, 148, 130));
        pushButton_KP2->setFont(font1);
        pushButton_KP3 = new QPushButton(KeyPad);
        pushButton_KP3->setObjectName(QString::fromUtf8("pushButton_KP3"));
        pushButton_KP3->setEnabled(false);
        pushButton_KP3->setGeometry(QRect(326, 130, 149, 130));
        pushButton_KP3->setFont(font1);
        pushButton_KP4 = new QPushButton(KeyPad);
        pushButton_KP4->setObjectName(QString::fromUtf8("pushButton_KP4"));
        pushButton_KP4->setEnabled(false);
        pushButton_KP4->setGeometry(QRect(484, 130, 148, 130));
        pushButton_KP4->setFont(font1);
        pushButton_KP5 = new QPushButton(KeyPad);
        pushButton_KP5->setObjectName(QString::fromUtf8("pushButton_KP5"));
        pushButton_KP5->setEnabled(false);
        pushButton_KP5->setGeometry(QRect(642, 130, 148, 130));
        pushButton_KP5->setFont(font1);
        pushButton_KP6 = new QPushButton(KeyPad);
        pushButton_KP6->setObjectName(QString::fromUtf8("pushButton_KP6"));
        pushButton_KP6->setEnabled(false);
        pushButton_KP6->setGeometry(QRect(10, 270, 148, 130));
        pushButton_KP6->setFont(font1);
        pushButton_KP7 = new QPushButton(KeyPad);
        pushButton_KP7->setObjectName(QString::fromUtf8("pushButton_KP7"));
        pushButton_KP7->setEnabled(false);
        pushButton_KP7->setGeometry(QRect(168, 270, 148, 130));
        pushButton_KP7->setFont(font1);
        pushButton_KP8 = new QPushButton(KeyPad);
        pushButton_KP8->setObjectName(QString::fromUtf8("pushButton_KP8"));
        pushButton_KP8->setEnabled(false);
        pushButton_KP8->setGeometry(QRect(326, 270, 148, 130));
        pushButton_KP8->setFont(font1);
        pushButton_KP9 = new QPushButton(KeyPad);
        pushButton_KP9->setObjectName(QString::fromUtf8("pushButton_KP9"));
        pushButton_KP9->setEnabled(false);
        pushButton_KP9->setGeometry(QRect(484, 270, 148, 130));
        pushButton_KP9->setFont(font1);
        pushButton_KP0 = new QPushButton(KeyPad);
        pushButton_KP0->setObjectName(QString::fromUtf8("pushButton_KP0"));
        pushButton_KP0->setEnabled(false);
        pushButton_KP0->setGeometry(QRect(642, 270, 148, 130));
        pushButton_KP0->setFont(font1);
        pushButton_KP_BS = new QPushButton(KeyPad);
        pushButton_KP_BS->setObjectName(QString::fromUtf8("pushButton_KP_BS"));
        pushButton_KP_BS->setEnabled(false);
        pushButton_KP_BS->setGeometry(QRect(10, 410, 210, 140));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Sans Serif"));
        font2.setPointSize(30);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setUnderline(false);
        font2.setWeight(50);
        font2.setStrikeOut(false);
        pushButton_KP_BS->setFont(font2);
        pushButton_KP_CLS = new QPushButton(KeyPad);
        pushButton_KP_CLS->setObjectName(QString::fromUtf8("pushButton_KP_CLS"));
        pushButton_KP_CLS->setEnabled(false);
        pushButton_KP_CLS->setGeometry(QRect(230, 410, 210, 140));
        pushButton_KP_CLS->setFont(font2);
        ButtonOk = new QPushButton(KeyPad);
        ButtonOk->setObjectName(QString::fromUtf8("ButtonOk"));
        ButtonOk->setEnabled(false);
        ButtonOk->setGeometry(QRect(450, 410, 341, 141));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Sans Serif"));
        font3.setPointSize(35);
        font3.setBold(false);
        font3.setItalic(false);
        font3.setUnderline(false);
        font3.setWeight(50);
        font3.setStrikeOut(false);
        ButtonOk->setFont(font3);
        lineEdit_MSG = new QLineEdit(KeyPad);
        lineEdit_MSG->setObjectName(QString::fromUtf8("lineEdit_MSG"));
        lineEdit_MSG->setEnabled(false);
        lineEdit_MSG->setGeometry(QRect(10, 50, 480, 60));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Arial"));
        font4.setPointSize(20);
        font4.setBold(false);
        font4.setItalic(false);
        font4.setUnderline(false);
        font4.setWeight(50);
        font4.setStrikeOut(false);
        lineEdit_MSG->setFont(font4);
        lineEdit_PIN = new QLineEdit(KeyPad);
        lineEdit_PIN->setObjectName(QString::fromUtf8("lineEdit_PIN"));
        lineEdit_PIN->setEnabled(false);
        lineEdit_PIN->setGeometry(QRect(520, 50, 240, 60));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Arial Black"));
        font5.setPointSize(30);
        font5.setBold(false);
        font5.setItalic(false);
        font5.setUnderline(false);
        font5.setWeight(50);
        font5.setStrikeOut(false);
        lineEdit_PIN->setFont(font5);
        lineEdit_PIN->setLayoutDirection(Qt::LeftToRight);
        lineEdit_PIN->setAlignment(Qt::AlignHCenter);

        retranslateUi(KeyPad);

        QMetaObject::connectSlotsByName(KeyPad);
    } // setupUi

    void retranslateUi(QDialog *KeyPad)
    {
        KeyPad->setWindowTitle(QApplication::translate("KeyPad", "Enter your pin!", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("KeyPad", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Entered Number</p></body></html>", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("KeyPad", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Message from User Interface</p></body></html>", 0, QApplication::UnicodeUTF8));
        pushButton_KP1->setText(QApplication::translate("KeyPad", "1", 0, QApplication::UnicodeUTF8));
        pushButton_KP2->setText(QApplication::translate("KeyPad", "2", 0, QApplication::UnicodeUTF8));
        pushButton_KP3->setText(QApplication::translate("KeyPad", "3", 0, QApplication::UnicodeUTF8));
        pushButton_KP4->setText(QApplication::translate("KeyPad", "4", 0, QApplication::UnicodeUTF8));
        pushButton_KP5->setText(QApplication::translate("KeyPad", "5", 0, QApplication::UnicodeUTF8));
        pushButton_KP6->setText(QApplication::translate("KeyPad", "6", 0, QApplication::UnicodeUTF8));
        pushButton_KP7->setText(QApplication::translate("KeyPad", "7", 0, QApplication::UnicodeUTF8));
        pushButton_KP8->setText(QApplication::translate("KeyPad", "8", 0, QApplication::UnicodeUTF8));
        pushButton_KP9->setText(QApplication::translate("KeyPad", "9", 0, QApplication::UnicodeUTF8));
        pushButton_KP0->setText(QApplication::translate("KeyPad", "0", 0, QApplication::UnicodeUTF8));
        pushButton_KP_BS->setText(QApplication::translate("KeyPad", "BS", 0, QApplication::UnicodeUTF8));
        pushButton_KP_CLS->setText(QApplication::translate("KeyPad", "CLS", 0, QApplication::UnicodeUTF8));
        ButtonOk->setText(QApplication::translate("KeyPad", "&Enter", 0, QApplication::UnicodeUTF8));
        lineEdit_MSG->setText(QApplication::translate("KeyPad", "Waiting the Omni's Connection", 0, QApplication::UnicodeUTF8));
        lineEdit_PIN->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class KeyPad: public Ui_KeyPad {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYPAD_H
