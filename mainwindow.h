#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QLabel>
#include <QLineEdit>
// HTTP available at wasm
#define WASM 0

#ifdef WASM
//#include <QWebSocket>
#include <QtWebSockets/QWebSocket>
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr,
                        Qt::WindowFlags flags = 0);

private slots:
    void wsStartTimer();
    void openFileBrowser();
    void fileOpenComplete(const QString &fileName, const QByteArray &data);
    void wsSendMsg();
    void wsOnConnected();
    void wsClosed();
    void wsTimerDisconnect();
    void wsTimerError(QAbstractSocket::SocketError error);
    void wsOnTextMessageReceived(QString message);

private:

    QPushButton *m_start_timer_btn;
    QPushButton *m_upload_file_btn;
    QPushButton *m_websocket_connect_button;
    QPushButton *m_websocket_disconnect_button;
    QLabel      *m_timer_messages_lbl;
    QLineEdit   *m_input_message_edt;
    QWebSocket  m_websocket_timer;
    QWebSocket  m_websocket_msg;
    QWebSocket  ws_uploadData;
    QNetworkAccessManager *net_mgr;
};
#endif // MAINWINDOW_H
