#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include "addcardialog.h"
#include <QInputDialog>
#include <QHeaderView>
#include <QFileDialog>
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , parkingManager(ParkingManager::getInstance())
    , isAdmin(false)
    , currentPage(0)
    , totalPages(0)
    , carTable(nullptr)
    , prevPageBtn(nullptr)
    , nextPageBtn(nullptr)
    , pageInfoLabel(nullptr)
{
    ui->setupUi(this);
    setupUI();
    showLoginDialog();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(createToolBar());

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->addWidget(createStatsPanel());

    // 创建一个垂直布局来包含表格和分页控件
    QVBoxLayout* tableLayout = new QVBoxLayout();
    
    // 初始化表格
    carTable = new QTableWidget(this);
    carTable->setColumnCount(5);
    carTable->setHorizontalHeaderLabels({"車牌號", "入場時間", "停車時長", "停車費用", "操作"});
    carTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    carTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    carTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    carTable->verticalHeader()->setDefaultSectionSize(50);
    
    tableLayout->addWidget(carTable);
    
    // 创建分页控件容器
    QWidget* paginationWidget = new QWidget(this);
    QHBoxLayout* paginationLayout = new QHBoxLayout(paginationWidget);
    
    // 初始化分页控件
    prevPageBtn = new QPushButton("上一頁", this);
    nextPageBtn = new QPushButton("下一頁", this);
    pageInfoLabel = new QLabel("第 1/1 頁", this);
    
    // 设置分页控件样式
    pageInfoLabel->setStyleSheet("QLabel { color: black; font-size: 14px; }");
    
    QSize buttonSize(100, 35);
    prevPageBtn->setFixedSize(buttonSize);
    nextPageBtn->setFixedSize(buttonSize);
    
    // 初始状态下禁用分页按钮
    prevPageBtn->setEnabled(false);
    nextPageBtn->setEnabled(false);
    
    // 连接分页按钮信号
    connect(prevPageBtn, &QPushButton::clicked, this, &MainWindow::onPrevPage);
    connect(nextPageBtn, &QPushButton::clicked, this, &MainWindow::onNextPage);
    
    paginationLayout->addStretch();
    paginationLayout->addWidget(prevPageBtn);
    paginationLayout->addWidget(pageInfoLabel);
    paginationLayout->addWidget(nextPageBtn);
    paginationLayout->addStretch();
    
    tableLayout->addWidget(paginationWidget);
    
    contentLayout->addLayout(tableLayout);
    contentLayout->setStretch(0, 1);
    contentLayout->setStretch(1, 3);

    mainLayout->addLayout(contentLayout);

    setCentralWidget(centralWidget);
    setStyleSheet(getStyleSheet());
    resize(1200, 800);

    // 修改表格样式
    carTable->setStyleSheet(
        "QTableWidget {"
        "   background-color: white;"
        "   border: 1px solid #ddd;"
        "}"
        "QTableWidget::item {"
        "   padding: 5px;"
        "   color: black;"
        "}"
        "QHeaderView::section {"
        "   background-color: #f0f0f0;"
        "   color: black;"
        "   padding: 5px;"
        "   border: 1px solid #ddd;"
        "}"
        "QTableWidget QHeaderView::section:vertical {"  // 垂直表头（序号列）的样式
        "   background-color: black;"     // 黑色背景
        "   color: white;"               // 白色文字
        "   padding: 5px;"
        "   border: none;"
        "   font-weight: normal;"        // 普通字重
        "}"
    );

    // 设置表格的选中项样式
    carTable->setAlternatingRowColors(true);
    carTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    carTable->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 设置垂直表头的宽度
    carTable->verticalHeader()->setDefaultSectionSize(50);
    carTable->verticalHeader()->setFixedWidth(40);  // 设置固定宽度
}
QWidget* MainWindow::createToolBar()
{
    QWidget* toolBar = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(toolBar);
    layout->setSpacing(10);

    QPushButton* refreshBtn = new QPushButton("刷新", this);
    QPushButton* searchBtn = new QPushButton("搜索", this);
    QPushButton* addBtn = new QPushButton("添加車輛", this);
    QPushButton* recognizeBtn = new QPushButton("車牌識別", this);
    QPushButton* logoutBtn = new QPushButton("登出", this);

    QSize buttonSize(100, 35);
    refreshBtn->setFixedSize(buttonSize);
    searchBtn->setFixedSize(buttonSize);
    addBtn->setFixedSize(buttonSize);
    recognizeBtn->setFixedSize(buttonSize);
    logoutBtn->setFixedSize(buttonSize);

    // 设置识别按钮的样式
    recognizeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;  /* 绿色背景 */
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
    )");

    // 连接信号
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshTable);
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchCar);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddCar);
    connect(recognizeBtn, &QPushButton::clicked, this, &MainWindow::onRecognizePlate);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogout);

    layout->addWidget(refreshBtn);
    layout->addWidget(searchBtn);
    layout->addWidget(addBtn);
    layout->addWidget(recognizeBtn);
    layout->addStretch();
    layout->addWidget(logoutBtn);

    addBtn->setVisible(isAdmin);

    toolBar->setLayout(layout);
    return toolBar;
}

QWidget* MainWindow::createStatsPanel()
{
    QWidget* panel = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    layout->addWidget(createStatsCard("總車位", "100"));
    layout->addWidget(createStatsCard("已停車輛",
        QString::number(parkingManager.getTotalCars())));
    layout->addWidget(createStatsCard("空餘車位",
        QString::number(100 - parkingManager.getTotalCars())));

    layout->addStretch();
    return panel;
}

QWidget* MainWindow::createStatsCard(const QString& title, const QString& value)
{
    QWidget* card = new QWidget(this);
    card->setObjectName("statsCard");

    QVBoxLayout* layout = new QVBoxLayout(card);

    QLabel* titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("statsTitle");

    QLabel* valueLabel = new QLabel(value, card);
    valueLabel->setObjectName("statsValue");

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);

    // 添加属性以标识统计卡片类型
    if (title.contains("已停車輛")) {
        card->setProperty("statsType", "parked");
    }

    return card;
}

QString MainWindow::getStyleSheet()
{
    return R"(
        QMainWindow {
            background-color: #f5f5f5;
        }

        QPushButton {
            padding: 8px 16px;
            border: none;
            border-radius: 4px;
            background-color: #2196F3;
            color: white;
            font-size: 14px;
        }

        QPushButton:hover {
            background-color: #1976D2;
        }

        QTableWidget {
            border: 1px solid #e0e0e0;
            border-radius: 4px;
            background-color: white;
            font-size: 14px;
        }

        QTableWidget::item {
            padding: 12px;
        }

        #statsCard {
            background-color: white;
            border-radius: 8px;
            padding: 16px;
            margin: 8px;
        }

        #statsTitle {
            color: #757575;
            font-size: 14px;
        }

        #statsValue {
            color: #212121;
            font-size: 24px;
            font-weight: bold;
        }

        QTableWidget QHeaderView::section {
            background-color: #f5f5f5;
            padding: 12px;
            font-size: 14px;
            font-weight: bold;
            border: none;
            border-bottom: 1px solid #e0e0e0;
        }

        QTableWidget QPushButton {
            min-width: 80px;
            min-height: 30px;
            font-size: 14px;
        }
    )";
}

void MainWindow::showLoginDialog()
{
    LoginWindow loginDialog(this);
    connect(&loginDialog, &LoginWindow::loginSuccess, this, &MainWindow::onLoginSuccess);

    if (loginDialog.exec() != QDialog::Accepted) {
        close();
    }
}

void MainWindow::onLoginSuccess(bool admin)
{
    this->isAdmin = admin;

    // 更新工具栏
    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    for(QPushButton* btn : buttons) {
        if(btn->text() == "添加車輛") {
            btn->setVisible(admin);
        }
    }
    
    // 刷新界面
    setWindowTitle(admin ? "澳門酒店停車場管理系統 - 管理員" : "澳門酒店停車場管理系統 - 用戶");
    updateCarTable();
}


void MainWindow::updateCarTable()
{
    QList<Car> allCars = parkingManager.getAllCars();
    totalPages = (allCars.size() + itemsPerPage - 1) / itemsPerPage;
    
    // 确保当前页在有效范围内
    if (currentPage >= totalPages) {
        currentPage = totalPages - 1;
    }
    if (currentPage < 0) {
        currentPage = 0;
    }

    // 更新分页按钮状态
    prevPageBtn->setEnabled(currentPage > 0);
    nextPageBtn->setEnabled(currentPage < totalPages - 1);
    
    // 更新页码信息
    pageInfoLabel->setText(QString("第 %1/%2 頁").arg(currentPage + 1).arg(totalPages));

    // 清空表格
    carTable->setRowCount(0);

    // 根据用户权限设置列数
    carTable->setColumnCount(isAdmin ? 5 : 4);  // 非管理员不显示操作列
    QStringList headers;
    headers << "車牌號" << "入場時間" << "停車時長" << "停車費用";
    if (isAdmin) {
        headers << "操作";
    }
    carTable->setHorizontalHeaderLabels(headers);

    // 计算当前页的数据范围
    int startIndex = currentPage * itemsPerPage;
    int endIndex = qMin(startIndex + itemsPerPage, allCars.size());

    // 只显示当前页的数据
    for (int i = startIndex; i < endIndex; i++) {
        const Car& car = allCars[i];
        int row = carTable->rowCount();
        carTable->insertRow(row);

        QTableWidgetItem* plateItem = new QTableWidgetItem(car.getPlateNumber());
        plateItem->setTextAlignment(Qt::AlignCenter);
        carTable->setItem(row, 0, plateItem);

        QTableWidgetItem* timeItem = new QTableWidgetItem(
            car.getEntryTime().toString("yyyy-MM-dd hh:mm:ss"));
        timeItem->setTextAlignment(Qt::AlignCenter);
        carTable->setItem(row, 1, timeItem);

        QDateTime now = QDateTime::currentDateTime();
        int hours = car.getEntryTime().secsTo(now) / 3600;
        QTableWidgetItem* durationItem = new QTableWidgetItem(QString::number(hours) + " 小時");
        durationItem->setTextAlignment(Qt::AlignCenter);
        carTable->setItem(row, 2, durationItem);

        double fee = car.calculateParkingFee();
        QTableWidgetItem* feeItem = new QTableWidgetItem(
            QString::number(fee, 'f', 2) + " MOP");
        feeItem->setTextAlignment(Qt::AlignCenter);
        carTable->setItem(row, 3, feeItem);

        // 只为管理员添加操作按钮
        if (isAdmin) {
            QWidget* buttonContainer = new QWidget();
            QHBoxLayout* layout = new QHBoxLayout(buttonContainer);
            
            QPushButton* editBtn = new QPushButton("編輯", this);
            QPushButton* deleteBtn = new QPushButton("刪除", this);
            
            QString buttonStyle = 
                "QPushButton {"
                "   padding: 5px;"
                "   color: white;"
                "   border: none;"
                "   font-size: 14px;"
                "   min-height: 30px;"
                "   min-width: 60px;"
                "   margin: 2px;"
                "}"
                "QPushButton:hover {"
                "   opacity: 0.8;"
                "}";
            
            editBtn->setStyleSheet(buttonStyle + "background-color: #2196F3;");
            deleteBtn->setStyleSheet(buttonStyle + "background-color: #f44336;");
            
            layout->addWidget(editBtn);
            layout->addWidget(deleteBtn);
            layout->setContentsMargins(2, 2, 2, 2);
            layout->setSpacing(4);
            
            carTable->setCellWidget(row, 4, buttonContainer);

            // 连接编辑按钮信号
            connect(editBtn, &QPushButton::clicked, [this, car]() {
                AddCarDialog dialog(this);
                dialog.setWindowTitle("編輯車輛信息");
                dialog.setCarInfo(car);

                if (dialog.exec() == QDialog::Accepted) {
                    Car updatedCar = dialog.getCar();
                    if (parkingManager.updateCar(car.getPlateNumber(), updatedCar)) {
                        updateCarTable();
                        QMessageBox::information(this, "成功", "車輛信息更新成功！");
                    } else {
                        QMessageBox::warning(this, "錯誤", "車輛信息更新失敗！");
                    }
                }
            });

            // 连接删除按钮信号
            connect(deleteBtn, &QPushButton::clicked, [this, car]() {
                QMessageBox::StandardButton reply = QMessageBox::question(
                    this, 
                    "確認刪除",
                    QString("確定要刪除車牌號為 %1 的車輛記錄嗎？").arg(car.getPlateNumber()),
                    QMessageBox::Yes | QMessageBox::No
                );

                if (reply == QMessageBox::Yes) {
                    if (parkingManager.removeCar(car.getPlateNumber())) {
                        updateCarTable();
                        QMessageBox::information(this, "成功", "車輛記錄已刪除！");
                    } else {
                        QMessageBox::warning(this, "錯誤", "刪除車輛記錄失敗！");
                    }
                }
            });
        }
    }

    // 更新统计面板
    updateStatsPanel();
}

// 添加新的辅助方法来更新统计面板
void MainWindow::updateStatsPanel()
{
    QList<QWidget*> cards = findChildren<QWidget*>("statsCard");
    for (QWidget* card : cards) {
        QLabel* titleLabel = card->findChild<QLabel*>("statsTitle");
        QLabel* valueLabel = card->findChild<QLabel*>("statsValue");
        
        if (titleLabel && valueLabel) {
            if (titleLabel->text() == "已停車輛") {
                valueLabel->setText(QString::number(parkingManager.getTotalCars()));
            } else if (titleLabel->text() == "空餘車位") {
                valueLabel->setText(QString::number(100 - parkingManager.getTotalCars()));
            }
        }
    }
}

void MainWindow::onAddCar()
{
    AddCarDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Car car = dialog.getCar();
        if (parkingManager.addCar(car)) {
            updateCarTable();  // 这个调用现在会同时更新表格和统计面板
            QMessageBox::information(this, "成", "車輛添加成功！");
        } else {
            QMessageBox::warning(this, "錯誤", "車輛添加失敗！");
        }
    }
}

void MainWindow::clearTableSelection()
{
    // 清除所有行的选中状态和背景色
    for(int row = 0; row < carTable->rowCount(); ++row) {
        for(int col = 0; col < carTable->columnCount(); ++col) {
            QTableWidgetItem* item = carTable->item(row, col);
            if(item) {
                item->setBackground(Qt::white);
            }
        }
    }
}

void MainWindow::onSearchCar()
{
    bool ok;
    QString plateNumber = QInputDialog::getText(
        this,
        "搜索車輛",
        "請輸入車牌號碼：",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (!ok || plateNumber.isEmpty()) {
        return;
    }

    // 获取所有车辆
    QList<Car> allCars = parkingManager.getAllCars();
    QList<Car> matchedCars;

    // 查找所有匹配的车辆
    for (const Car& car : allCars) {
        if (car.getPlateNumber().contains(plateNumber, Qt::CaseInsensitive)) {
            matchedCars.append(car);
        }
    }

    if (matchedCars.isEmpty()) {
        QMessageBox::information(this, "搜索結果", "未找到匹配的車輛");
        return;
    }

    // 创建并显示搜索结果对话框
    QDialog* resultDialog = new QDialog(this);
    resultDialog->setWindowTitle("搜索結果");
    resultDialog->setMinimumWidth(600);

    QVBoxLayout* layout = new QVBoxLayout(resultDialog);

    // 创建表格显示搜索结果
    QTableWidget* resultTable = new QTableWidget(resultDialog);
    resultTable->setColumnCount(4);
    resultTable->setHorizontalHeaderLabels({"車牌號", "入場時間", "停車時長", "停車費用"});
    resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 添加搜索结果到表格
    for (const Car& car : matchedCars) {
        int row = resultTable->rowCount();
        resultTable->insertRow(row);

        // 创建表格项并设置文字颜色和对齐方式
        QTableWidgetItem* plateItem = new QTableWidgetItem(car.getPlateNumber());
        plateItem->setTextAlignment(Qt::AlignCenter);
        plateItem->setForeground(QBrush(Qt::black));
        resultTable->setItem(row, 0, plateItem);

        QTableWidgetItem* timeItem = new QTableWidgetItem(
            car.getEntryTime().toString("yyyy-MM-dd hh:mm:ss"));
        timeItem->setTextAlignment(Qt::AlignCenter);
        timeItem->setForeground(QBrush(Qt::black));
        resultTable->setItem(row, 1, timeItem);

        QTableWidgetItem* durationItem = new QTableWidgetItem(
            QString::number(car.getEntryTime().secsTo(QDateTime::currentDateTime()) / 3600) + " 小時");
        durationItem->setTextAlignment(Qt::AlignCenter);
        durationItem->setForeground(QBrush(Qt::black));
        resultTable->setItem(row, 2, durationItem);

        QTableWidgetItem* feeItem = new QTableWidgetItem(
            QString::number(car.calculateParkingFee(), 'f', 2) + " MOP");
        feeItem->setTextAlignment(Qt::AlignCenter);
        feeItem->setForeground(QBrush(Qt::black));
        resultTable->setItem(row, 3, feeItem);
    }

    // 添加标签显示搜索结果数量，设置白色文字
    QLabel* countLabel = new QLabel(QString("找到 %1 輛匹配的車輛").arg(matchedCars.size()), resultDialog);
    countLabel->setStyleSheet("QLabel { color: white; font-size: 14px; }");
    layout->addWidget(countLabel);
    layout->addWidget(resultTable);

    // 添加关闭按钮
    QPushButton* closeBtn = new QPushButton("關閉", resultDialog);
    connect(closeBtn, &QPushButton::clicked, resultDialog, &QDialog::accept);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    layout->addLayout(btnLayout);

    // 更新对话框样式
    resultDialog->setStyleSheet(R"(
        QDialog {
            background-color: #333333;  /* 深色背景 */
        }
        QTableWidget {
            border: 1px solid #ddd;
            background-color: white;
        }
        QTableWidget::item {
            padding: 5px;
            color: black;  /* 表格内容保持黑色 */
        }
        QHeaderView::section {
            background-color: #f0f0f0;
            padding: 5px;
            border: 1px solid #ddd;
            color: black;  /* 表头文字保持黑色 */
            font-weight: bold;
        }
        QLabel {
            color: white;  /* 标签文字设为白色 */
            font-size: 14px;
        }
        QPushButton {
            min-width: 80px;
            padding: 5px 15px;
            background-color: #2196F3;
            color: white;
            border: none;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #1976D2;
        }
    )");

    resultDialog->exec();
    delete resultDialog;
}

void MainWindow::onRefreshTable()
{
    updateCarTable();
}

void MainWindow::onLogout()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "確認登出",
        "確定要退出登錄嗎？",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // 保存当前数据
        parkingManager.saveToJson();
        
        // 清空当前数据
        carTable->setRowCount(0);
        currentPage = 0;
        totalPages = 0;
        isAdmin = false;
        
        // 更新页码显示
        pageInfoLabel->setText("第 1/1 頁");
        prevPageBtn->setEnabled(false);
        nextPageBtn->setEnabled(false);
        
        // 隐藏主窗口而不是关闭它
        this->hide();
        
        // 显示登录对话框
        LoginWindow* loginDialog = new LoginWindow(this);
        connect(loginDialog, &LoginWindow::loginSuccess, this, [this](bool admin) {
            // 登录成功后的处理
            this->isAdmin = admin;
            
            // 更新工具栏
            QList<QPushButton*> buttons = findChildren<QPushButton*>();
            for(QPushButton* btn : buttons) {
                if(btn->text() == "添加車輛" || btn->text() == "車牌識別") {
                    btn->setVisible(admin);
                }
            }
            
            // 刷新界面
            setWindowTitle(admin ? "澳門酒店停車場管理系統 - 管理員" : "澳門酒店停車場管理系統 - 用戶");
            updateCarTable();
            
            // 显示主窗口
            this->show();
        });

        if (loginDialog->exec() != QDialog::Accepted) {
            // 如果用户取消登录，则关闭程序
            this->close();
        }
        
        delete loginDialog;
    }
}

void MainWindow::onPrevPage()
{
    if (currentPage > 0) {
        currentPage--;
        updateCarTable();
    }
}

void MainWindow::onNextPage()
{
    if (currentPage < totalPages - 1) {
        currentPage++;
        updateCarTable();
    }
}

void MainWindow::onRecognizePlate()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "選擇圖片", 
        QDir::homePath(),  // 从用户主目录开始
        "圖片文件 (*.jpg *.jpeg *.png)");
    
    if (fileName.isEmpty()) {
        return;
    }

    // 使用 try-catch 包装图片读取操作
    cv::Mat image;
    try {
        image = cv::imread(fileName.toStdString());
        if (image.empty()) {
            QMessageBox::warning(this, "錯誤", "無法讀取圖片！");
            return;
        }
    }
    catch (const cv::Exception& e) {
        QMessageBox::warning(this, "錯誤", 
            QString("圖片處理錯誤：%1").arg(e.what()));
        return;
    }

    // 创建预览对话框
    QDialog* previewDialog = new QDialog(this);
    previewDialog->setWindowTitle("車牌識別結果");
    
    // 使用网格布局，更高效
    QGridLayout* layout = new QGridLayout(previewDialog);

    // 显示原图（优化图片显示）
    QLabel* imageLabel = new QLabel;
    QImage qImage;
    
    try {
        // 转换图片格式
        qImage = QImage(image.data, image.cols, image.rows,
                       image.step, QImage::Format_BGR888);
        
        // 智能缩放
        QSize screenSize = QApplication::primaryScreen()->size();
        QSize maxSize(screenSize.width() * 0.7, screenSize.height() * 0.7);
        QPixmap pixmap = QPixmap::fromImage(qImage);
        
        if (pixmap.width() > maxSize.width() || 
            pixmap.height() > maxSize.height()) {
            pixmap = pixmap.scaled(maxSize, Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation);
        }
        
        imageLabel->setPixmap(pixmap);
    }
    catch (const std::exception& e) {
        QMessageBox::warning(this, "錯誤", 
            QString("圖片處理錯誤：%1").arg(e.what()));
        delete previewDialog;
        return;
    }

    // 使用网格布局添加控件
    layout->addWidget(new QLabel("原始圖片："), 0, 0);
    layout->addWidget(imageLabel, 1, 0, 1, 2);

    QString plateNumber = recognizePlate(image);
    QLabel* resultLabel = new QLabel("識別結果：" + plateNumber);
    resultLabel->setStyleSheet(
        "QLabel { font-size: 16px; font-weight: bold; }");
    layout->addWidget(resultLabel, 2, 0, 1, 2);

    // 添加按钮
    QPushButton* addButton = new QPushButton("添加到系統");
    QPushButton* closeButton = new QPushButton("關閉");
    
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout, 3, 0, 1, 2);

    // 连接按钮信号
    connect(addButton, &QPushButton::clicked,
            [this, plateNumber, previewDialog]() {
        Car newCar(plateNumber);
        if (parkingManager.addCar(newCar)) {
            updateCarTable();
            QMessageBox::information(this, "成功", "車輛添加成功！");
            previewDialog->accept();
        } else {
            QMessageBox::warning(this, "錯誤", "車輛已存在或添加失敗！");
        }
    });

    connect(closeButton, &QPushButton::clicked,
            previewDialog, &QDialog::reject);

    previewDialog->exec();
    delete previewDialog;
}

QString MainWindow::recognizePlate(const cv::Mat& image)
{
    try {
        // 1. 图像预处理 - 只进行必要的操作
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        
        // 2. 使用自适应阈值，效果更好且速度更快
        cv::Mat binary;
        cv::adaptiveThreshold(gray, binary, 255,
                            cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                            cv::THRESH_BINARY, 11, 2);

        // 3. 轮廓检测
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binary, contours, cv::RETR_LIST,
                        cv::CHAIN_APPROX_SIMPLE);

        // 4. 返回示例车牌号
        return "MA-12-34";
    }
    catch (const cv::Exception& e) {
        qDebug() << "OpenCV error:" << e.what();
        return "MA-00-00";
    }
}