#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>  // For logging
#include <QThread>
#include "worker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSendButtonClicked();   // To send data to RabbitMQ
    void onMessageReceived(const QString &message);  // Slot to handle received messages
    void logMessage(const QString &message);  // Slot to log connection status

private:
    QListWidget *listWidget;
    QPushButton *sendButton;
    QTextEdit *logWidget;  // Widget for displaying logs

    Worker *worker;
    QThread *workerThread;

    void sendRabbitMQMessage(const std::string &message);  // Helper function to send a message to RabbitMQ
};

#endif // MAINWINDOW_H
