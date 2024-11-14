#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QKeyEvent>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void loginSuccess(bool isAdmin);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void on_loginButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::LoginWindow *ui;
    bool m_isAdmin;
    bool validateCredentials(const QString& username, const QString& password);
    QString hashPassword(const QString& password);
};

#endif // LOGINWINDOW_H
