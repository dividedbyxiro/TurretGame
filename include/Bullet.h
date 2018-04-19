#ifndef BULLET_H
#define BULLET_H


class Bullet
{
    public:
        Bullet();
        virtual ~Bullet();
        void move();
        void kill();
        double getXPos();
        double getYPos();
        double getXVel();
        double getYVel();
        void setVelocity(double x, double y);
        void setPosition(double x, double y);
        bool getAlive();
        void setAlive(bool instruction);

    protected:

    private:
        double xPos;
        double yPos;
        double xVel;
        double yVel;
        bool alive;
};

#endif // BULLET_H
