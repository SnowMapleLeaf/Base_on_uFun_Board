#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QRandomGenerator>
#include <QSerialPortInfo>
#include <QDebug>

//#define DEBUG_cout(info) qDebug()<<info
#define DEBUG_cout(info)

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setFixedSize(block_width * x_count, block_height * y_count);
    this->curDirection = dir_right;
    this->preDirection = this->curDirection;
    this->startFlag = false;
    pSnake = new Snake(QPoint(x_count / 2, y_count / 2), 5, curDirection);

    InitSerialPort();
    // 产生第一个食物
    makeFood();

    pTimer = new QTimer(this);
    pTimer->setInterval(150);           // 控制重绘的时间间隔，可以控制游戏难度
    connect(pTimer, &QTimer::timeout, this,
            [=](){
                // 如果串口没有连接成功，则一直尝试
                if(pSerialPort->isOpen() == false){
                    DEBUG_cout("still try");
                    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();
                    // 如果连接上一个串口直接跳出，没有可用串口会在程序中不断循环查找串口并尝试连接
                    QSerialPortInfo info;
                    foreach(info, infoList){
                        pSerialPort->setPortName(info.portName());
                        if(pSerialPort->open(QIODevice::ReadWrite))
                            break;
                    }
                }
                if(this->startFlag == true){
                    // 只有每次绘制的时候才会真正修改蛇的前进方向
                    this->curDirection = this->preDirection;
                    // 判断是否增加长度以及是否游戏结束
                    if(this->isGameOver()){
                        // 游戏结束后清除标志变量，以进行下一场游戏
                        this->startFlag = false;
                        pSnake = new Snake(QPoint(x_count / 2, y_count / 2), 5, curDirection);
                        makeFood();
                    }
                    else {
                        QPoint point = pSnake->getHeadLocation();
                        pSnake->tryGo(&point, this->curDirection);
                        if(point == foodPoint){             // 判断有没有吃到食物
                            pSerialPort->write("<F>");      // 发送<F>表示吃到食物
                            pSnake->addBlock(this->curDirection);
                            // 更新食物的位置
                            this->makeFood();
                        }
                        else{
                            pSnake->makeStep(curDirection);
                        }
                    }
                    this->update();
                }
    });
    pTimer->start();
}

Widget::~Widget()
{
    delete ui;
}
void Widget::InitSerialPort()
{
    pSerialPort = new QSerialPort(this);
    rcvData.clear();
    frameFlag = false;

    pSerialPort->setBaudRate(QSerialPort::Baud115200);
    pSerialPort->setDataBits(QSerialPort::Data8);
    pSerialPort->setStopBits(QSerialPort::StopBits::OneStop);
    pSerialPort->setParity(QSerialPort::NoParity);
    pSerialPort->setFlowControl(QSerialPort::NoFlowControl);
    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();
    // 如果连接上一个串口直接跳出，没有可用串口会在程序中不断循环查找串口并尝试连接
    QSerialPortInfo info;
    foreach(info, infoList){
        pSerialPort->setPortName(info.portName());
        if(pSerialPort->open(QIODevice::ReadWrite))
            break;
    }

    // 处理设备被意外拔出
    connect(pSerialPort, &QSerialPort::errorOccurred, this,
            [=](QSerialPort::SerialPortError error){
                if(error != QSerialPort::NoError){
                    if(pSerialPort->isOpen())
                        pSerialPort->close();
                    DEBUG_cout("disconnect");
                }
            });
    // 处理接收数据
    connect(pSerialPort, &QSerialPort::readyRead, this,
            [=](){
                char ch;
                while(pSerialPort->read(&ch, 1) == 1){
                    if(ch == '<'){
                        frameFlag = true;
                        rcvData.clear();
                    }
                    else if(frameFlag == true){
                        if(ch == '>'){
                            // 一帧接收结束
                            // 当停在游戏开始界面时，检测到按键就直接置位开始游戏
                            if(this->startFlag == false){
                                this->startFlag = true;
                                if(pTimer->isActive() == false)
                                    pTimer->start();
                                return;
                            }
                            // A-left   D-right W-up    S-down
                            switch (rcvData.toUtf8().at(0)) {
                                case 'A': case 'a':
                                    if(this->curDirection != dir_right)
                                        this->preDirection = dir_left;
                                    break;
                                case 'D': case 'd':
                                    if(this->curDirection != dir_left)
                                        this->preDirection = dir_right;
                                    break;
                                case 'W': case 'w':
                                    if(this->curDirection != dir_down)
                                        this->preDirection = dir_up;
                                    break;
                                case 'S': case 's':
                                    if(this->curDirection != dir_up)
                                        this->preDirection = dir_down;
                                    break;
                            }
                            frameFlag = false;
                        }
                        else {
                            rcvData.append(ch);
                        }
                    }
                }
            });
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QColor("#ff0000"));

    QList<QPoint> snake_body = pSnake->getBody();
    QPoint point;
    foreach(point, snake_body){
        painter.drawRect(point.x() * block_width, point.y() * block_height, block_width, block_height);
    }
    painter.drawEllipse(foodPoint.x() * block_width, foodPoint.y() * block_height, block_width, block_height);
    if(this->startFlag == false){
        QFont font = painter.font();
        font.setPixelSize(block_height * 3);
        painter.setFont(font);
        painter.drawText(this->rect(), Qt::AlignCenter, "按任意键开始游戏");
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Q){
        this->close();
    }

    if(this->startFlag == false){
        this->startFlag = true;
        if(pTimer->isActive() == false)
            pTimer->start();
        return;
    }
    switch (event->key()) {
        case Qt::Key_A: case Qt::Key_Left:
            if(this->curDirection != dir_right)
                this->preDirection = dir_left;
            break;
        case Qt::Key_D: case Qt::Key_Right:
            if(this->curDirection != dir_left)
                this->preDirection = dir_right;
            break;
        case Qt::Key_W: case Qt::Key_Up:
            if(this->curDirection != dir_down)
                this->preDirection = dir_up;
            break;
        case Qt::Key_S: case Qt::Key_Down:
            if(this->curDirection != dir_up)
                this->preDirection = dir_down;
            break;
    }
}

void Widget::makeFood()
{
    QRandomGenerator generator;
    quint32 x, y;
    while(true){
        generator.seed(static_cast<quint32>(QTime::currentTime().second()));
        x = generator.generate();
        y = generator.generate();
        x %= x_count;
        y %= y_count;
        if(pSnake->isInSnake(static_cast<int>(x), static_cast<int>(y)) == false)
            break;
    }
    foodPoint.setX(static_cast<int>(x));
    foodPoint.setY(static_cast<int>(y));
    DEBUG_cout(x << y) ;
}
bool Widget::isGameOver()
{
    QPoint point = pSnake->getHeadLocation();
    pSnake->tryGo(&point, this->curDirection);
    if(pSnake->isInSnake(point))
        return true;
    if(point.x() < 0 || point.x() >= x_count)
        return true;
    if(point.y() < 0 || point.y() >= y_count)
        return true;
    return false;
}
