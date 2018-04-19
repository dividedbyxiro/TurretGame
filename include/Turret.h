#ifndef TURRET_H
#define TURRET_H


class Turret
{
    public:
        Turret();
        virtual ~Turret();
        void move();
        void setStopped(bool instruction);
        void setPosition(int x, int y);
        int getHealth();
        int getXPos();
        int getYPos();
        int getAngle();
        int getWidth();
        int getHeight();

    protected:

    private:
        int angle;
        int health;
        int xPos;
        int yPos;
        int direction;
        bool stopped;
        int width;
        int height;
};

#endif // TURRET_H
