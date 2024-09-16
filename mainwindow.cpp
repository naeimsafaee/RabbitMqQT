#include "mainwindow.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Create the list widget
    listWidget = new QListWidget(this);

    // Create the send button
    sendButton = new QPushButton("Send", this);

    // Create the log widget
    logWidget = new QTextEdit(this);
    logWidget->setReadOnly(true);  // Make log widget read-only

    // Connect the button click to the slot
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);

    // Create a layout and add the widgets
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(listWidget);
    layout->addWidget(sendButton);
    layout->addWidget(logWidget);  // Add log widget to the layout

    // Create a central widget and set the layout
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);


    // Set up the worker and thread for background processing
    workerThread = new QThread(this);
    worker = new Worker();
    worker->moveToThread(workerThread);

    // Connect the worker's message signal to the MainWindow slot
    connect(worker, &Worker::messageReceived, this, &MainWindow::onMessageReceived);

    // Connect a custom signal for logging connection status
    connect(worker, &Worker::logMessage, this, &MainWindow::logMessage);

    // Start listening for messages
    connect(workerThread, &QThread::started, worker, &Worker::listenToQueue);

    // Stop the worker thread when the window is closed
    connect(this, &QMainWindow::destroyed, worker, &Worker::stopListening);
    connect(workerThread, &QThread::finished, worker, &Worker::deleteLater);

    // Start the worker thread
    worker->setupConnection();
    workerThread->start();
}

MainWindow::~MainWindow() {
    workerThread->quit();
    workerThread->wait();
}

// Slot to receive messages from the Worker and display them in the list
void MainWindow::onMessageReceived(const QString &message) {
    static int messageCount = 1;
    QString numberedMessage = QString::number(messageCount++) + ". " + message;
    listWidget->addItem(numberedMessage);
}

// Slot to log connection status
void MainWindow::logMessage(const QString &message) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    logWidget->append("[" + timestamp + "] " + message);
}

// Send data to RabbitMQ when the "Send" button is clicked
void MainWindow::onSendButtonClicked() {
    QString message = "Test message";  // You can replace this with a user-defined message
    sendRabbitMQMessage(message.toStdString());
}

// Send a message to RabbitMQ
void MainWindow::sendRabbitMQMessage(const std::string &message) {
    worker->sendMessage(QString::fromStdString(message));
}
