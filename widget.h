#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QString encode(int type, int data_type, const char*, const char*);
    QString decode(int type, int data_type, const char*, const char*);
};
#endif // WIDGET_H
