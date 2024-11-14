#include "parkingmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

ParkingManager& ParkingManager::getInstance() {
    static ParkingManager instance;
    return instance;
}

ParkingManager::ParkingManager() {
    loadFromJson();
}

bool ParkingManager::addCar(const Car& car) {
    if (cars.contains(car.getPlateNumber())) {
        return false;
    }
    cars.insert(car.getPlateNumber(), car);
    saveToJson();
    return true;
}

// bool ParkingManager::removeCar(const QString& plateNumber) {
//     if (!cars.contains(plateNumber)) {
//         return false;
//     }
//     cars.remove(plateNumber);
//     saveToJson();
//     return true;
// }

Car* ParkingManager::findCar(const QString& plateNumber) {
    if (!cars.contains(plateNumber)) {
        return nullptr;
    }
    return &cars[plateNumber];
}

bool ParkingManager::updateCar(const QString& plateNumber, const Car& newInfo) {
    if (!cars.contains(plateNumber)) {
        return false;
    }
    cars[plateNumber] = newInfo;
    saveToJson();
    return true;
}

QList<Car> ParkingManager::getAllCars() const {
    return cars.values();
}

int ParkingManager::getTotalCars() const {
    return cars.size();
}

void ParkingManager::saveToJson() const {
    QJsonArray carsArray;
    for (const Car& car : cars) {
        QJsonObject carObject;
        carObject["plateNumber"] = car.getPlateNumber();
        carObject["entryTime"] = car.getEntryTime().toString(Qt::ISODate);
        carsArray.append(carObject);
    }

    QJsonDocument doc(carsArray);
    QFile file(JSON_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}
bool ParkingManager::removeCar(const QString& plateNumber)
{
    if (cars.contains(plateNumber)) {
        cars.remove(plateNumber);
        saveToJson();  // 注意这里使用 saveToJson 而不是 saveToFile
        return true;
    }
    return false;
}
void ParkingManager::loadFromJson() {
    QFile file(JSON_FILE);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray carsArray = doc.array();

        cars.clear();
        for (const QJsonValue& value : carsArray) {
            QJsonObject obj = value.toObject();
            Car car(obj["plateNumber"].toString());
            car.setEntryTime(QDateTime::fromString(obj["entryTime"].toString(), Qt::ISODate));
            cars.insert(car.getPlateNumber(), car);
        }
        file.close();
    }
}
