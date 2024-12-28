#ifndef DECLARE_H
#define DECLARE_H

#include <QWidget>

namespace Ui {
class declare;
}

class declare : public QWidget
{
    Q_OBJECT

public:
    explicit declare(QWidget *parent = nullptr);
    ~declare();

private slots:
    void on_BTN_AGREE_clicked();

    void on_pushButton_clicked();

private:
    Ui::declare *ui;
};

#endif // DECLARE_H
