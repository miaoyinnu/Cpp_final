#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QRegularExpression>
#include <opencv2/opencv.hpp>
#include "parkingmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginSuccess(bool isAdmin);
    void onAddCar();
    void onSearchCar();
    void onRefreshTable();
    void onLogout();
    void onPrevPage();
    void onNextPage();
    void onRecognizePlate();

private:
    void setupUI();
    void updateStatsPanel();
    QWidget* createToolBar();
    QWidget* createStatsPanel();
    QWidget* createStatsCard(const QString& title, const QString& value);
    QString getStyleSheet();
    void onEditCar(const Car& car);
    void showLoginDialog();
    void updateCarTable();
    void clearTableSelection();
    QString recognizePlate(const cv::Mat& image);
    bool isValidPlateNumber(const QString& number);
    char recognizeCharacter(const cv::Mat& charImg);

    Ui::MainWindow *ui;
    QTableWidget *carTable;
    ParkingManager &parkingManager;
    bool isAdmin;
    
    int currentPage = 0;
    static const int itemsPerPage = 10;
    QPushButton* prevPageBtn;
    QPushButton* nextPageBtn;
    QLabel* pageInfoLabel;
    int totalPages = 0;
};

#endif // MAINWINDOW_H 