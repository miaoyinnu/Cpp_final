#include "car.h"

Car::Car() : entryTime(QDateTime::currentDateTime()) {}

Car::Car(const QString& plateNumber) 
    : plateNumber(plateNumber), entryTime(QDateTime::currentDateTime()) {}

QString Car::getPlateNumber() const {
    return plateNumber;
}

void Car::setPlateNumber(const QString& plateNumber) {
    this->plateNumber = plateNumber;
}

QDateTime Car::getEntryTime() const {
    return entryTime;
}

void Car::setEntryTime(const QDateTime& time) {
    entryTime = time;
}

double Car::calculateParkingFee() const
{
    QDateTime now = QDateTime::currentDateTime();
    int totalHours = getEntryTime().secsTo(now) / 3600;  // 转换为小时
    
    // 24小时内按每小时20MOP计算，超过24小时封顶400MOP
    if (totalHours < 20) {
        return totalHours * 20.0;
    } else {
        return 400.0;  // 封顶价格
    }
}
