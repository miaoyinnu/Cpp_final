#include "addcardialog.h"
#include <QMessageBox>
#include <QRegularExpression>

AddCarDialog::AddCarDialog(QWidget *parent) : QDialog(parent)
{
    setupUI();
}

void AddCarDialog::setupUI()
{
    setWindowTitle("添加車輛");
    setFixedSize(300, 200);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 車牌號輸入
    QHBoxLayout* plateLayout = new QHBoxLayout();
    QLabel* plateLabel = new QLabel("車牌號：", this);
    plateNumberEdit = new QLineEdit(this);
    plateLayout->addWidget(plateLabel);
    plateLayout->addWidget(plateNumberEdit);
    mainLayout->addLayout(plateLayout);

    // 入場時間
    QHBoxLayout* timeLayout = new QHBoxLayout();
    QLabel* timeLabel = new QLabel("入場時間：", this);
    entryTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(entryTimeEdit);
    mainLayout->addLayout(timeLayout);

    // 按鈕
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* confirmBtn = new QPushButton("確認", this);
    QPushButton* cancelBtn = new QPushButton("取消", this);
    buttonLayout->addWidget(confirmBtn);
    buttonLayout->addWidget(cancelBtn);
    mainLayout->addLayout(buttonLayout);

    // 連接信號
    connect(confirmBtn, &QPushButton::clicked, this, &AddCarDialog::onConfirm);
    connect(cancelBtn, &QPushButton::clicked, this, &AddCarDialog::onCancel);
}

void AddCarDialog::onConfirm()
{
    QString plateNumber = plateNumberEdit->text().trimmed();
    if (plateNumber.isEmpty()) {
        QMessageBox::warning(this, "錯誤", "請輸入車牌號！");
        return;
    }

    if (!validatePlateNumber(plateNumber)) {
        return;
    }

    accept();
}

void AddCarDialog::onCancel()
{
    reject();
}

Car AddCarDialog::getCar() const
{
    Car car;
    car.setPlateNumber(plateNumberEdit->text());
    car.setEntryTime(entryTimeEdit->dateTime());
    return car;
}
void AddCarDialog::setCarInfo(const Car& car)
{
    plateNumberEdit->setText(car.getPlateNumber());
    entryTimeEdit->setDateTime(car.getEntryTime());
}

bool AddCarDialog::validatePlateNumber(const QString& plateNumber)
{
    // M后面必须跟一个字母，然后是-XX-XX格式，XX可以是数字或字母
    QRegularExpression regExp("^M[A-Z]-[A-Z0-9]{2}-[A-Z0-9]{2}$");
    QRegularExpressionMatch match = regExp.match(plateNumber);
    
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "錯誤",
            "車牌號碼格式不正確！\n"
            "正確格式示例：\n"
            "- MA-12-34\n"
            "- MB-AB-12\n"
            "- MC-12-CD\n"
            "註：第二個字符必須為字母，後面的字符可以是字母或數字");
        return false;
    }
    return true;
}