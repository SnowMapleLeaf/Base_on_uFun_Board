#ifndef SNAKE_H
#define SNAKE_H
#include <QPoint>
#include <QList>
#define dir_up      1
#define dir_down    2
#define dir_left    3
#define dir_right   4
class Snake
{
public:
    Snake(QPoint headLocation, int lenth = 3, int cur_dir = dir_right);

    int getLenth();             // 获取贪吃蛇长度
    int getCurDirecton();       // 获取当前前进方向
    QList<QPoint>& getBody();
    QPoint getHeadLocation();   // 获取贪吃蛇头部的坐标位置
    bool addBlock(int curDir);            // 添加一块身体
    bool makeStep(int dir);     // 向某个方向前进一步

    bool isInSnake(QPoint& point);          // 判断某一点是否在蛇的身体内
    bool isInSnake(int x, int y);           // 判断某一点是否在蛇的身体内
    void tryGo(QPoint* point, int dir);     // 获取走一步之后的新坐标点
private:
    int curDirection;
    QList<QPoint> snake_body;
};

#endif // SNAKE_H
