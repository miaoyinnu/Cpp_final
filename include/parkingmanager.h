#ifndef PARKINGMANAGER_H
#define PARKINGMANAGER_H

#include <QObject>
#include <QMap>
#include "car.h"

class ParkingManager : public QObject {
    Q_OBJECT
public:
    static ParkingManager& getInstance();
    bool removeCar(const QString& plateNumber); 
    bool addCar(const Car& car);
    Car* findCar(const QString& plateNumber);
    bool updateCar(const QString& plateNumber, const Car& newInfo);
    QList<Car> getAllCars() const;
    int getTotalCars() const;
    void saveToJson() const;
    int getParkedCount() const {
        return cars.size();  // 返回当前停车场内的车辆数量
    }
    void loadFromJson();
    

private:
    ParkingManager();
    QMap<QString, Car> cars;
    const QString JSON_FILE = "parking_data.json";
};

#endif // PARKINGMANAGER_H
