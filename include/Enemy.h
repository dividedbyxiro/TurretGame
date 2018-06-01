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
        void setAlive(int value);
        int getHealth();
        int getSize();  /**< returns radius determined by type */
        bool damage(int amount);
        int getState();
        void setType(int value);
        int getType();

    protected:

    private:
    	int health;
    	double xPos;
        double yPos;
        double xVel;
        double yVel;
        bool alive;
        int state;      //time since damaged, controls which sprite is rendered
        int type;
};

#endif // ENEMY_H
