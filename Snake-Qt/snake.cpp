#include "snake.h"

Snake::Snake(QPoint headLocation, int lenth, int dir)
{
    if(lenth == 0)  lenth = 1;
    this->curDirection = dir;
    this->snake_body.clear();
    // 如果给的初始头位置有问题
    if(headLocation.x() < 0 || headLocation.y() < 0){
        headLocation.setX(lenth);
        headLocation.setY(lenth);
    }
    // 填充蛇的身体列表
    QPoint tmpP = headLocation;
    do{
        snake_body.append(tmpP);
        switch (dir) {
            case dir_up:
                tmpP.setY(tmpP.y() + 1);
                break;
            case dir_down:
                tmpP.setY(tmpP.y() - 1);
                break;
            case dir_left:
                tmpP.setX(tmpP.x() + 1);
                break;
            case dir_right:
                tmpP.setX(tmpP.x() - 1);
                break;
        }
    }while (snake_body.length() < lenth);
}

int Snake::getLenth(){
    return snake_body.length();
}

int Snake::getCurDirecton(){
    return this->curDirection;
}

QPoint Snake::getHeadLocation(){
    return snake_body.first();
}

QList<QPoint>& Snake::getBody()
{
    return snake_body;
}

bool Snake::makeStep(int dir){
    this->curDirection = dir;       // 更新方向
    QPoint newHead = snake_body.first();
    switch (dir) {
        case dir_up:
            newHead.setY(newHead.y() - 1);
            break;
        case dir_down:
            newHead.setY(newHead.y() + 1);
            break;
        case dir_left:
            newHead.setX(newHead.x() - 1);
            break;
        case dir_right:
            newHead.setX(newHead.x() + 1);
            break;
    }
    snake_body.push_front(newHead);
    snake_body.pop_back();
    return true;
}

bool Snake::addBlock(int curDir){
    this->curDirection = curDir;
    QPoint newHead = snake_body.first();
    switch (curDirection) {
        case dir_up:
            newHead.setY(newHead.y() - 1);
            break;
        case dir_down:
            newHead.setY(newHead.y() + 1);
            break;
        case dir_left:
            newHead.setX(newHead.x() - 1);
            break;
        case dir_right:
            newHead.setX(newHead.x() + 1);
            break;
    }
    snake_body.push_front(newHead);

    return true;
}

bool Snake::isInSnake(QPoint& point)
{
    if(snake_body.indexOf(point) == -1){
        return false;
    }
    return true;
}
bool Snake::isInSnake(int x, int y)
{
    QPoint point(x, y);
    if(snake_body.indexOf(point) == -1){
        return false;
    }
    return true;
}

void Snake::tryGo(QPoint* point, int dir)
{
    switch (dir) {
        case dir_up:
            point->setY(point->y() - 1);
            break;
        case dir_down:
            point->setY(point->y() + 1);
            break;
        case dir_left:
            point->setX(point->x() - 1);
            break;
        case dir_right:
            point->setX(point->x() + 1);
            break;
    }
}

