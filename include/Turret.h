#ifndef TURRET_H
#define TURRET_H


class Turret
{
    public:
        Turret();
        virtual ~Turret();
        void move();
        void stop();
        void start();
        void setPosition(int x, int y);
        void setDirection(double newDirection);
        int getXPos();
        int getYPos();
        double getAngle();
        int getWidth();
        int getHeight();
        double getDirection();
        bool isStopped();
    protected:

    private:
        double angle;
        int xPos;
        int yPos;
        double direction;
        bool stopped;
        int width;
        int height;
};

#endif // TURRET_H
