#ifndef ENEMY_H
#define ENEMY_H


class Enemy
{
    public:
        Enemy();
        virtual ~Enemy();
        void move();
        void kill();
        double getXPos();
        double getYPos();
        double getXVel();
        double getYVel();
        void setVelocity(double x, double y);
        void setPosition(double x, double y);
        bool getAlive();
        void setAlive();
        int getHealth();
        void setSize(int newSize);
        int getSize();
        bool damage(int amount);

    protected:

    private:
    	int health;
    	double xPos;
        double yPos;
        double xVel;
        double yVel;
        int size;
        bool alive;
};

#endif // ENEMY_H
