#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include "snake.h"

#define block_width     15
#define block_height    15
#define x_count         50
#define y_count         40

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void makeFood();        // 更新食物的位置
    bool isGameOver();      // 判断游戏是否结束
private:
    QSerialPort *pSerialPort;
    QString rcvData;                // 存储串口接收到的数据
    bool frameFlag;                 // 用来标记自定义的一帧数据开始
    void InitSerialPort();          // 初始化串口设置
private:
    bool startFlag;         // 标志游戏是否正在进行中
    QTimer* pTimer;         // 游戏开始后用来定时重绘的计时器
    QPoint foodPoint;       // 记录食物的位置
    Snake *pSnake;          // 贪吃蛇对象
    int curDirection;       // 记录当前行进方向
    int preDirection;
    Ui::Widget *ui;
};

#endif // WIDGET_H
