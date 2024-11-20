#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QKeyEvent>

LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow),
    m_isAdmin(false)
{
    ui->setupUi(this);
    setWindowTitle("澳門酒店停車場管理系統 - 登錄");
    
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::on_loginButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &LoginWindow::on_cancelButton_clicked);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (validateCredentials(username, password)) {
        emit loginSuccess(m_isAdmin);
        accept();
    } else {
        QMessageBox::warning(this, "錯誤", "用戶名或密碼錯誤！");
    }
}

void LoginWindow::on_cancelButton_clicked()
{
    reject();
}

QString LoginWindow::hashPassword(const QString& password)
{
    QByteArray passwordData = password.toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256);
    return hashedPassword.toHex();
}

bool LoginWindow::validateCredentials(const QString& username, const QString& password)
{
    const QString ADMIN_PASS_HASH = "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb"; // "a" 的 SHA-256 哈希值
    const QString USER_PASS_HASH = "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb";  // "u" 的 SHA-256 哈希值

    QString hashedInputPass = hashPassword(password);

    if (username == "admin" && hashedInputPass == ADMIN_PASS_HASH) {
        m_isAdmin = true;
        return true;
    }
    else if (username == "user" && hashedInputPass == USER_PASS_HASH) {
        m_isAdmin = false;
        return true;
    }
    return false;
}

void LoginWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        event->ignore();
        return;
    }
    QDialog::keyPressEvent(event);
}
