#include "mainwindow.h"
#include <QDebug>
#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QObject>
#include <QMetaEnum>
#include <QNetworkAccessManager>
#if 0
#include <QHttpPart>
#endif

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)

{
    this->resize(500, 400);

    connect(&m_websocket_timer, &QWebSocket::connected, this, &MainWindow::wsOnConnected);
    connect(&m_websocket_timer, &QWebSocket::disconnected, this, &MainWindow::wsClosed);
    connect(&m_websocket_timer, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &MainWindow::wsTimerError);
    connect(&m_websocket_timer, &QWebSocket::textMessageReceived, this, &MainWindow::wsOnTextMessageReceived);

    /*
     * SEND MESSAGE
     */

    m_websocket_connect_button = new QPushButton("Send message to Server", this);
    m_websocket_connect_button->setGeometry((QRect(QPoint(30, 30), QSize(200, 50))));
    connect(m_websocket_connect_button, &QAbstractButton::released, this, &MainWindow::wsSendMsg );

    /*
     * DISCONNECT WEBSOCKET
     */

    m_websocket_disconnect_button = new QPushButton("Disconnect Websocket", this);
    m_websocket_disconnect_button->setGeometry((QRect(QPoint(30, 170), QSize(200, 50))));
    connect(m_websocket_disconnect_button, SIGNAL(released()), this, SLOT(wsTimerDisconnect()) );

    /*
     * UPLOAD FILE BUTTON
     */

    m_upload_file_btn = new QPushButton("Upload File", this);
    m_upload_file_btn->setGeometry((QRect(QPoint(30, 240), QSize(200, 50))));
    connect(m_upload_file_btn, SIGNAL(released()), this, SLOT(openFileBrowser()) );

    /*
     *  START TIMER
     */

    m_start_timer_btn = new QPushButton("Start Timer", this);
    m_start_timer_btn->setGeometry(QRect(QPoint(30, 100), QSize(200, 50)));
    connect(m_start_timer_btn, &QAbstractButton::released, this, &MainWindow::wsStartTimer );

    /*
     * TEXT INPUT
     */

    m_input_message_edt = new QLineEdit(this);
    m_input_message_edt->setGeometry(280, 40, 200, 30);
    m_input_message_edt->setPlaceholderText("custom message");

    /*
     * TIMER MESSAGES
     */

    m_timer_messages_lbl = new QLabel("Messages from server timer", this);
    m_timer_messages_lbl->setGeometry(280, 110, 200, 30);

}


void MainWindow::wsStartTimer(){
    qDebug() << "MainWindow::wsStartTimer() called";

    QUrl ws_url(QStringLiteral("ws://localhost:7000/timer"));
    qDebug() << "Open Websocket:: " << ws_url.toString();
    m_websocket_timer.open(ws_url);

}


void MainWindow::openFileBrowser(){
    qDebug() << "MainWindow::openFileBrowser() called";
    QString s_homePath = QDir::homePath();

#if 1
    QUrl ws_url(QStringLiteral("ws://localhost:7000/data"));
    ws_uploadData.open(ws_url);
    connect(&ws_uploadData, &QWebSocket::connected, []{ qDebug() << "wsUplData_onConnected() called"; });
    connect(&ws_uploadData, &QWebSocket::disconnected, []{ qDebug() << "wsUplData_onClosed() called"; });
#endif

    auto fileOpenCompleted = [this](const QString &filePath, const QByteArray &fileContent) {
        if (filePath.isEmpty() && !m_websocket_msg.isValid()) {
            qDebug() << "No file was selected";
        } else {
            qDebug() << "Size of file: " << fileContent.size() / 1000 << "kb";
            qDebug() << "Selected file: " << filePath;
            QFileInfo fileName(filePath);

#if 1       // Websocket variant
            //QByteArray uploadData(fileName.fileName().toUtf8());
            ws_uploadData.sendTextMessage(fileName.fileName());
            ws_uploadData.sendBinaryMessage(fileContent);
#else
            // Alternative: HTTP Multipart POST
            QString content_header = QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileName.fileName());
            QHttpPart fileDataPart;
            fileDataPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(content_header));
            fileDataPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
            fileDataPart.setBody(fileContent);

            QUrl url("http://localhost:7000/upload");
            QNetworkRequest qnet_req(url);

            QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
            multipart->append(fileDataPart);
            //QByteArray data(multipart->boundary());
            QByteArray data;
            data.append(fileContent);
            net_mgr.post(qnet_req, data);
            //QNetworkReply *reply = net_mgr.post(qnet_req, multipart);
#endif
        }
    };

    QFileDialog::getOpenFileContent("", fileOpenCompleted);
}


void MainWindow::fileOpenComplete(const QString &fileName, const QByteArray &data){
    qDebug() << "MainWindow::fileOpenComplete() called";
    qDebug() << "Filename: " << fileName;
    qDebug() << "Size: " << data.size();
}

void MainWindow::wsSendMsg()
{
    qDebug() << "MainWindow::wsSendMsg() called";
    QUrl ws_url(QStringLiteral("ws://localhost:7000/message"));
    qDebug() << "Open ws URL: " << ws_url.toString();



    auto ws_opened = [this]() {
        if(m_websocket_msg.isValid()){
           qDebug() << "Send text to server: " << m_input_message_edt->text();
           m_websocket_msg.sendTextMessage(m_input_message_edt->text());
           m_websocket_msg.close(QWebSocketProtocol::CloseCodeNormal,"Operation complete - closed by client");
        } else {
            qDebug() << "Websocket is NOT valid" ;
            m_websocket_msg.close(QWebSocketProtocol::CloseCodeAbnormalDisconnection,"Operation FAILED - closed");
        }
    };


    connect(&m_websocket_msg, &QWebSocket::connected, ws_opened);
    m_websocket_msg.open(ws_url);
}

void MainWindow::wsOnConnected()
{
    qDebug() << "MainWindow::wsOnConnected() called";
}

void MainWindow::wsClosed()
{
    qDebug() << "MainWindow::wsClosed() called";
}

void MainWindow::wsTimerDisconnect()
{
    qDebug() << "MainWindow::wsDisconnect() called";
    m_websocket_timer.close(QWebSocketProtocol::CloseCodeNormal,"Closed by User");
    m_timer_messages_lbl->setText("Disconnected");
}

void MainWindow::wsTimerError(QAbstractSocket::SocketError error)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    qDebug() << "WS Error: " << metaEnum.valueToKey(error);
    m_timer_messages_lbl->setText(metaEnum.valueToKey(error));
}


void MainWindow::wsOnTextMessageReceived(QString message)
{
    qDebug() << "MainWindow::wsOnTextMessageReceived: " << message;
    m_timer_messages_lbl->setText(message);
}
