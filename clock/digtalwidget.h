#ifndef DIGTALWIDGET_H
#define DIGTALWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QPalette>
#include <QColor>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QPixmap>
#include <QHBoxLayout>
#include <QIcon>
#include <QPaintEvent>
#include <QPainter>
#include <QAction>
#include <QBitmap>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QFile>

class DigtalWidget;
class NumberManger;

#define NUMSFIXEDWIDTH 30
#define NUMSFIXEDHEIGHT NUMSFIXEDWIDTH*2
#define FRAMEFIXWIDTH NUMSFIXEDWIDTH*11
#define FRAMEFIXHEIGHT NUMSFIXEDWIDTH*3
#define SETTING_FULLINFOM ".\\clocksetttings\\settings.ini"
#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

class DigtalWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DigtalWidget(QWidget *parent = 0);
    ~DigtalWidget();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
private slots:
    void showTimer();
    void changeStyle();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void changeAutoStart();
    void quitAndSet();
private:
    void initItem();
    void addRightMenu();
    void createTrayMenu();
    void createTrayIcon();
    void writeSettings();
    void readSettings();
    void setAutoStart();
    
    QPoint dragPositon;
    QList<QLabel *> timeNumsLabelList;
    QHBoxLayout *mainLayout;
    NumberManger *numMan;
    QTimer *numTimer;
    QPixmap frameBackground;
    QAction *rightMenu_Close,*rightMenu_Hide,*trayMenu_AutoStart,
            *trayMenu_Close,*trayMenu_Normal, *trayMenu_Hide;
    QList<QAction *> rightMenu_Style;
    
    QMenu *trayMenu;
    QSystemTrayIcon *trayIcon;
    
    bool if_auto_start = false;
};

class NumberManger
{
    friend class DigtalWidget;
public:
    NumberManger();
    NumberManger(const QString &);
    void setStyle(const QString &);
    void setStyle(QString &&);
    
private:
    QStringList styleList{"Flat","Pink"};
    QList<QPixmap> nums;
    QPixmap cutStyle;
    QString nowStyle = "Flat";
};


#endif // DIGTALWIDGET_H
















