#ifndef ADDCARDIALOG_H
#define ADDCARDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QDateTimeEdit>
#include "car.h"

class AddCarDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddCarDialog(QWidget *parent = nullptr);
    Car getCar() const;
    void setCarInfo(const Car& car);  // 添加这个方法声明

    private slots:
    void onConfirm();
    void onCancel();

private:
    QLineEdit* plateNumberEdit;
    QDateTimeEdit* entryTimeEdit;
    QLineEdit *plateInput;

    void setupUI();
    bool validatePlateNumber(const QString& plateNumber);
};

#endif // ADDCARDIALOG_H