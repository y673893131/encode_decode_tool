#include "widget.h"
#include <QApplication>
#include <QCryptographicHash>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QDebug>
#include "Encrypt\Encrypt.h"
#include <QStyle>
enum
{
    Enc_Rc4,
    Enc_Aes,
    Enc_Des,
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    setWindowTitle("encode/decode tool");
    resize(800, 600);
    auto label_key = new QLabel("input key:",this);
    auto label_encode_type = new QLabel("encode type:",this);
    auto label_data_encode_type = new QLabel("data encode type:",this);
    auto edit_key = new QLineEdit(this);
    edit_key->setPlaceholderText("encode/decode key");
    auto edit_src = new QTextEdit(this);
    edit_src->setPlaceholderText("encode src string");
    auto edit_dst = new QTextEdit(this);
    edit_dst->setPlaceholderText("decode src string");
    auto encrypt = new QPushButton("encrypt", this);
    auto decrypt = new QPushButton("decrypt", this);
    auto clear = new QPushButton("clear", this);
    auto combobox_enc = new QComboBox(this);
    auto combobox_data_enc = new QComboBox(this);
    QStringList lists;
    lists << "rc4" << "aes" << "des";
    combobox_enc->addItems(lists);

    QStringList lists_data;
    lists_data << "hex" << "base64";
    combobox_data_enc->addItems(lists_data);

    int row = 0;
    auto layout1 = new QGridLayout(this);
    layout1->addWidget(edit_src, row++, 0, 1, 5);
    layout1->addWidget(label_key, row, 0);
    layout1->addWidget(edit_key, row++, 1);
    layout1->addWidget(label_data_encode_type, row, 0);
    layout1->addWidget(combobox_data_enc, row++, 1);
    layout1->addWidget(label_encode_type, row, 0);
    layout1->addWidget(combobox_enc, row, 1);
    layout1->addWidget(encrypt, row, 2);
    layout1->addWidget(decrypt, row, 3);
    layout1->addWidget(clear, row++, 4);
    layout1->addWidget(edit_dst, row++, 0, 1, 5);

    connect(clear, &QPushButton::clicked, this, [=]{
        edit_src->clear();
        edit_dst->clear();
    });

    connect(encrypt, &QPushButton::clicked, this, [=]{
        auto text = edit_src->toPlainText();
        auto lists = text.split('\n');
        QString sTmp;
        for(auto it : lists)
        {
            auto pSrc = (it + "\n").toLocal8Bit();
            auto pKey = edit_key->text().toLocal8Bit();
            auto sEncode = encode(combobox_enc->currentIndex(), combobox_data_enc->currentIndex(), pSrc, pKey);
            if(!sEncode.isEmpty())
            {
                sTmp += sEncode + "\n";
            }
        }

        if(!sTmp.isEmpty())
        {
            sTmp.remove(sTmp.length() - 1, 1);
        }

        edit_dst->setText(sTmp);
    });

    connect(decrypt, &QPushButton::clicked, this, [=]{
        auto text = edit_dst->toPlainText();
        auto lists = text.split('\n');
        QString sTmp;
        for(auto it : lists)
        {
            auto pSrc = it.toLocal8Bit();
            auto pKey = edit_key->text().toLocal8Bit();
            auto sDecode = decode(combobox_enc->currentIndex(), combobox_data_enc->currentIndex(),pSrc, pKey);
            sTmp += sDecode;
        }

        if(!sTmp.isEmpty())
        {
            sTmp.remove(sTmp.length() - 1, 1);
        }

        edit_src->setText(sTmp);
    });
}

Widget::~Widget()
{
}

QString Widget::encode(int type, int data_type,const char* src, const char* key)
{
    QString sRet;
    char* s = nullptr;
    switch (type) {
    case Enc_Rc4:
        s = Encrypt::RC4::encode(src, key, (Encrypt::Data_Type)data_type);
        break;
    case Enc_Des:
        s = Encrypt::DES::encode(src, key, (Encrypt::Data_Type)data_type);
        break;
    case Enc_Aes:
        s = Encrypt::AES::encode(src, key, (Encrypt::Data_Type)data_type);
        break;
    }

    if(s)
    {
        if(Enc_Rc4 != type)
        {
            sRet = QString::fromStdString(/*Encrypt::Base64::encode64(s)*/s);
        }
        else
        {
            sRet = QString::fromStdString(s);
        }

        delete[] s;
    }


    return sRet;
}

QString Widget::decode(int type,int data_type, const char* src, const char* key)
{
    QString sRet;
    char* s = nullptr;
    std::string sData;

    if(Enc_Rc4 != type)
    {
        sData = /*Encrypt::Base64::decode64(src)*/src;
    }
    else
    {
        sData = src;
    }

    switch (type) {
    case Enc_Rc4:
        s = Encrypt::RC4::decode(sData.c_str(), key, (Encrypt::Data_Type)data_type);
        break;
    case Enc_Des:
        s = Encrypt::DES::decode(sData.c_str(), key, (Encrypt::Data_Type)data_type);
        break;
    case Enc_Aes:
        s = Encrypt::AES::decode(sData.c_str(), key, (Encrypt::Data_Type)data_type);
        break;
    }

    if(s)
    {
        sRet = QString::fromLocal8Bit(s);
        delete[] s;
    }

//    qDebug() << QString::fromLocal8Bit(src) << sRet;

    return sRet;
}

