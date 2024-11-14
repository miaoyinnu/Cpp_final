#ifndef CAR_H
#define CAR_H

#include <QString>
#include <QDateTime>

class Car {
public:
    Car();
    Car(const QString& plateNumber);

    QString getPlateNumber() const;
    void setPlateNumber(const QString& plateNumber);
    QDateTime getEntryTime() const;
    void setEntryTime(const QDateTime& time);
    double calculateParkingFee() const;

private:
    QString plateNumber;
    QDateTime entryTime;
    static constexpr double HOURLY_RATE = 20.0;  // MOP per hour
    static constexpr double DAILY_RATE = 400.0;  // MOP per day
};

#endif // CAR_H
