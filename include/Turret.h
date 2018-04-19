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
        int getXPos();
        int getYPos();
        int getAngle();
        int getWidth();
        int getHeight();

    protected:

    private:
        int angle;
        int xPos;
        int yPos;
        int direction;
        bool stopped;
        int width;
        int height;
};

#endif // TURRET_H
