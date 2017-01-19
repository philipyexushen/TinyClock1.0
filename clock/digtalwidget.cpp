#include "digtalwidget.h"

DigtalWidget::DigtalWidget(QWidget *parent) 
    : QWidget(parent)
{
    //...............
    move(1552, 82);//设定初始位置
    initItem();
    createTrayIcon();
    readSettings();
    
    frameBackground.load(tr(":/num/digitalNums/frame.png"),
                         0,//默认加载模式
                         Qt::AvoidDither|Qt::ThresholdAlphaDither|Qt::ThresholdDither);
    frameBackground = frameBackground.scaled(FRAMEFIXWIDTH,FRAMEFIXHEIGHT);
    
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool|Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.95);//设置整体窗口透明度
    setWindowIcon(QIcon(tr(":/images/clockIcon.png")));
    setWindowTitle(tr("My Clock"));
    setMask(frameBackground.mask());
    //...............
    addRightMenu();
    setContextMenuPolicy(Qt::ActionsContextMenu);//开启右键菜单

    showTimer();
    resize(frameBackground.size());
}

void DigtalWidget::initItem()
{
    numMan = new NumberManger("Flat");
    numTimer = new QTimer(this);
    connect(numTimer,SIGNAL(timeout()), this, SLOT(showTimer()));
    numTimer->start(1000);//1000ms启动计时器
    
    for(int i = 0;i < 8;i++)
        timeNumsLabelList << new QLabel("");
    
    mainLayout = new QHBoxLayout(this);
    mainLayout->addSpacing(18);
    for(int i = 0;i < 8;i++)
        mainLayout->addWidget(timeNumsLabelList[i]);
    mainLayout->addSpacing(25);
    mainLayout->setMargin(10);
}

void DigtalWidget::addRightMenu()
{
    for(const auto &name : numMan->styleList)
    {
        QIcon icon(QObject::tr(":/num/digitalNums/") + name + tr("/rightMenuIcon.png"));
        QString showName(name + tr(" style"));
        QAction *tmp = new QAction(icon,showName,this);
        rightMenu_Style << tmp;
        addAction(tmp);
        connect(tmp, SIGNAL(triggered()), this, SLOT(changeStyle()));
    }
    
    rightMenu_Hide = new QAction(QIcon(":/pushButtons/pushButtons/showMinimized.png"),tr("隐藏"),this);
    addAction(rightMenu_Hide);
    connect(rightMenu_Hide,SIGNAL(triggered()),this,SLOT(hide()));
    
    rightMenu_Close = new QAction(QIcon(":/pushButtons/pushButtons/close.png"),tr("关闭"),this);
    addAction(rightMenu_Close);
    connect(rightMenu_Close, SIGNAL(triggered()),this,SLOT(quitAndSet()));
}

void DigtalWidget::createTrayIcon()
{
    if(!QSystemTrayIcon::isSystemTrayAvailable())//如果不支持系统托盘图标就退出
        return;
    createTrayMenu();
    trayIcon = new QSystemTrayIcon;
    trayIcon->setIcon(QIcon(":/images/clockIcon.png"));
    
    trayIcon->setToolTip(tr("Philip's clock V1.0"));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
    
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void DigtalWidget::createTrayMenu()
{
    trayMenu = new QMenu(this);
    
    trayMenu_AutoStart = new QAction(tr("程序自启动"),this);
    trayMenu->addAction(trayMenu_AutoStart);
    connect(trayMenu_AutoStart,SIGNAL(triggered()),this,SLOT(changeAutoStart()));
    
    trayMenu_Hide = new QAction(QIcon(":/pushButtons/pushButtons/hide.png"),tr("隐藏"),this);
    trayMenu->addAction(trayMenu_Hide);
    connect(trayMenu_Hide,SIGNAL(triggered()),this,SLOT(hide()));
    
    trayMenu_Normal = new QAction(QIcon(":/pushButtons/pushButtons/normal.png"),tr("显示"),this);
    trayMenu->addAction(trayMenu_Normal);
    connect(trayMenu_Normal,SIGNAL(triggered()),this,SLOT(showNormal()));
    
    trayMenu->addSeparator();//插一个分隔符
    
    trayMenu_Close = new QAction(QIcon(":/pushButtons/pushButtons/close.png"),tr("关闭"),this);
    trayMenu->addAction(trayMenu_Close);
    connect(trayMenu_Close, SIGNAL(triggered()),this,SLOT(quitAndSet()));//注意任务栏图标的关闭必须使用qApp的才能关
}

void DigtalWidget::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) 
    {
    case QSystemTrayIcon::DoubleClick:case QSystemTrayIcon::Trigger:
        showNormal();
        break;
    case QSystemTrayIcon::MiddleClick:
        trayIcon->showMessage(tr("From Philip:"),tr("Hi!"));
        break;
    default:
        break;
    }
}

void DigtalWidget::changeAutoStart()
{
    if_auto_start = !if_auto_start;
    setAutoStart();
}

void DigtalWidget::setAutoStart()
{
    QString appName = QApplication::applicationName();
    QSettings regSettings(REG_RUN,QSettings::NativeFormat);
    
    if(!if_auto_start)//去除自启动信息
    {
        trayMenu_AutoStart->setIcon(QIcon(""));
        regSettings.remove(appName);
    }
    else//添加信息
    {
        QString appPath = QApplication::applicationFilePath();
        regSettings.setValue(appName,appPath.replace("/","\\"));
        trayMenu_AutoStart->setIcon(QIcon(":/images/check.png"));
    }
}

void DigtalWidget::writeSettings()
{
    QSettings appBaseSettings(tr(SETTING_FULLINFOM),QSettings::IniFormat);//表示读取的是ini文件
    appBaseSettings.setValue("/Geometry",saveGeometry());
    appBaseSettings.setValue("/ifAutoRuning",if_auto_start);
    appBaseSettings.setValue("/style",numMan->nowStyle);
}

void DigtalWidget::readSettings()
{
    QFile file(tr(SETTING_FULLINFOM));
    if(!file.open(QIODevice::ReadOnly))
        return;//不存在配置文件
    
    QSettings appBaseSettings(tr(SETTING_FULLINFOM),QSettings::IniFormat);//表示读取的是ini文件
    
    restoreGeometry(appBaseSettings.value("/Geometry").toByteArray());
    
    if_auto_start = appBaseSettings.value("/ifAutoRuning").toBool();
    setAutoStart();
    
    numMan->nowStyle = appBaseSettings.value("/style").toString();
    numMan->setStyle(numMan->nowStyle);
}

void DigtalWidget::quitAndSet()
{
    writeSettings();
    QApplication::quit();
}

void DigtalWidget::showTimer()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm:ss");
    
    for(int i = 0; i < text.count(); i++)
    {
        if(text[i] != ':')
        {
            int k = text[i].cell() - '0';
            timeNumsLabelList[i]->setPixmap(numMan->nums[k]);
        }
        else
            timeNumsLabelList[i]->setPixmap(numMan->cutStyle);
    }
}

void DigtalWidget::changeStyle()
{
    QAction *catchAction = qobject_cast<QAction *>(sender());
    auto name = catchAction->text();
    name.replace(" style" , "", Qt::CaseSensitive);
    numMan->setStyle(name);
    showTimer();//强行刷新一次时间，去除延迟
}

void DigtalWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)    
    {
        dragPositon = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void DigtalWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - dragPositon);
        event->accept();
    }
}

void DigtalWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,frameBackground);
}

DigtalWidget::~DigtalWidget()
{
    delete numMan;
}

NumberManger::NumberManger()
{
    setStyle(nowStyle);
}

NumberManger::NumberManger(const QString &style)
{
    setStyle(style);
}

void NumberManger::setStyle(const QString &style)
{
    nums.clear();
    for(int i = 0;i < 10;i++)
    {
        QString s(QObject::tr(":/num/digitalNums/") + style + QObject::tr("/%1.png").arg(i));
        QPixmap m(s);
        m = m.scaled(NUMSFIXEDWIDTH,NUMSFIXEDHEIGHT,Qt::KeepAspectRatio);
        nums << m;
    }
    QString s(QObject::tr(":/num/digitalNums/") + style + QObject::tr("/cut.png"));
    QPixmap m(s);
    cutStyle = m.scaled((int)(NUMSFIXEDWIDTH/1.5),(int)(NUMSFIXEDHEIGHT/1.5));
    nowStyle = style;
}

void NumberManger::setStyle(QString &&style)
{
    nums.clear();
    for(int i = 0;i < 10;i++)
    {
        QString s(QObject::tr(":/num/digitalNums/") + style + QObject::tr("/%1.png").arg(i));
        QPixmap m(s);
        m = m.scaled(NUMSFIXEDWIDTH,NUMSFIXEDHEIGHT,Qt::KeepAspectRatio);
        nums << m;
    }
    QString s(QObject::tr(":/num/digitalNums/") + style + QObject::tr("/cut.png"));
    QPixmap m(s);
    cutStyle = m.scaled((int)(NUMSFIXEDWIDTH/1.5),(int)(NUMSFIXEDHEIGHT/1.5));
    nowStyle = style;
}






