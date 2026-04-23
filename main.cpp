#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

// --------------------------------------------------
float scaleFactor = 1.0f;
float headAngle = 0.0f;
float handMove = 0.0f;
float legMove = 0.0f;

float robotX = 0.0f;
float robotY = -60.0f;

float cloudX1 = -300;
float cloudX2 = 100;

float sunY = 200.0f;
float sunDirection = -1.0f;   // start moving down

float busX = -450.0f;   // start from left side
float busSpeed = 2.0f;

// --------------------------------------------------
void drawPixel(int x, int y)
{
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// --------------------------------------------------
void DDA(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float Xinc = (float)dx / steps;
    float Yinc = (float)dy / steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++)
    {
        drawPixel((int)round(x), (int)round(y));
        x += Xinc;
        y += Yinc;
    }
}

// --------------------------------------------------
void Bresenham(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (1)
    {
        drawPixel(x1, y1);

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

// --------------------------------------------------
// FILLED RECTANGLE
void fillRectangle(int x, int y, int w, int h)
{
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();
}

// FILLED TRIANGLE
void fillTriangle(int x1,int y1,int x2,int y2,int x3,int y3)
{
    glBegin(GL_TRIANGLES);
    glVertex2i(x1,y1);
    glVertex2i(x2,y2);
    glVertex2i(x3,y3);
    glEnd();
}

// FILLED CIRCLE
void fillCircle(int xc, int yc, int r)
{
    glBegin(GL_POLYGON);
    for(int i=0;i<360;i++)
    {
        float theta = i * 3.1416 / 180;
        glVertex2i(xc + r*cos(theta), yc + r*sin(theta));
    }
    glEnd();
}

// --------------------------------------------------
void drawHouse(int x, int y)
{

    // Gray house
    glColor3f(0.6,0.6,0.6);
    fillRectangle(x,y,80,60);

    // Blue roof
    glColor3f(0.2,0.4,1.0);
    fillTriangle(x-10,y+60,x+40,y+100,x+90,y+60);

    // Door
    glColor3f(0.3,0.2,0.1);
    fillRectangle(x+30,y,20,35);

    // Windows
    glColor3f(0.7,0.4,2.0);
    fillRectangle(x+10,y+25,15,15);
    fillRectangle(x+55,y+25,15,15);

    glColor3f(0.7,0.7,0.7);
fillRectangle(x+35, y-20, 10, 20);
}

// --------------------------------------------------
void drawTree(int x, int y)
{
    // trunk
    glColor3f(0.55,0.27,0.07);
    fillRectangle(x,y,20,50);

    // leaves
    glColor3f(0.0,0.8,0.0);
    fillCircle(x+10,y+70,20);
    fillCircle(x-10,y+60,18);
    fillCircle(x+30,y+60,18);
}

// --------------------------------------------------
void drawRoad()
{
    glColor3f(0.3,0.3,0.3);
    fillRectangle(-400,-200,800,120);

    glColor3f(1,1,1);
    for(int i=-350;i<350;i+=80)
        fillRectangle(i,-145,40,5);
}


void drawfootpath()
{
    glColor3f(0.1, 0.6, 0.0);
    fillRectangle(-400,-100,800,-200);
}

// --------------------------------------------------
void drawRobot()
{
    glPushMatrix();

    glTranslatef(robotX, robotY, 0);
    glScalef(scaleFactor, scaleFactor, 1.0);

   glPushMatrix();

    glTranslatef(0, 80, 0);           // Move to head center
    glRotatef(headAngle, 0, 0, 1);    // Rotate head

    // Head
    glColor3f(0,0,0);
    fillCircle(0, 0, 25);

    // Eyes (RECTANGLES)
    glColor3f(1,1,1);                 // white eyes
    fillRectangle(-12, 5, 8, 6);      // left eye
    fillRectangle(4, 5, 8, 6);        // right eye

    // Pupils
    glColor3f(0,0,0);
    fillRectangle(-9, 6, 3, 3);       // left pupil
    fillRectangle(7, 6, 3, 3);        // right pupil

    // Mouth (RECTANGLE)
    glColor3f(1,0,0);
    fillRectangle(-8, -10, 16, 5);    // mouth

    glPopMatrix();
    // BODY
    glColor3f(0, 0,0);   // blue color
    fillRectangle(-25,0,50,60);

    // HANDS
    //glColor3f(1.0, 0.0, 1.0);   // blue color
    //glTranslatef(-25 - handMove, 40, 0);
    fillRectangle(-37,20,10,35);
    fillRectangle(27,20,10,35);
    fillRectangle(-37,42,17,15);
    fillRectangle(20,42,17,15);

    // LEGS
    fillRectangle(-18,-40,10,40);
    fillRectangle(10,-40,10,40);

    glPopMatrix();
}

// --------------------------------------------------
void update(int value)
{

    busX += busSpeed;

// reset when it goes out of screen
if(busX > 650)
    busX = -450;

//if(busX < 300)
 //   busSpeed = -busSpeed;
   // if(busX > 450)

    // Slow up-down movement
sunY += 0.1f * sunDirection;

// Reverse direction at limits
if(sunY > 260) sunDirection = -1.0f;
if(sunY < -260) sunDirection = 1.0f;

    cloudX1 += 0.5;
cloudX2 += 0.3;

// reset when off screen
if(cloudX1 > 400) cloudX1 = -400;
if(cloudX2 > 400) cloudX2 = -400;

    handMove = 5 * sin(robotX * 0.05);
    legMove = 5 * cos(robotX * 0.05);

    glutPostRedisplay();
    glutTimerFunc(30, update, 0);
}
// --------------------------------------------------
void drawSun()
{
    // Sun body
    glColor3f(1.0, 0.9, 0.0);
    fillCircle(300, sunY, 40);

    // Rays
    glColor3f(1.0, 0.8, 0.0);

    for(int i = 0; i < 360; i += 30)
    {
        float angle = i * 3.1416 / 180;

        int x1 = 300 + 40 * cos(angle);
        int y1 = sunY + 40 * sin(angle);

        int x2 = 300 + 60 * cos(angle);
        int y2 = sunY + 60 * sin(angle);

        Bresenham(x1, y1, x2, y2);
    }
}

// --------------------------------------------------
void drawCloud(float x, float y)
{
    glColor3f(1.0, 1.0, 1.0); // white clouds

    fillCircle(x, y, 20);
    fillCircle(x+20, y+10, 25);
    fillCircle(x+40, y, 20);
    fillCircle(x+20, y-10, 18);
}

// --------------------------------------------------
void drawHill()
{
    // Big hill (center)
    glColor3f(0.0, 0.6, 0.0);
    fillTriangle(-300, -80, 0, 180, 300, -80);

    // Left hill
    glColor3f(0.1, 0.5, 0.0);
    fillTriangle(-400, -80, -200, 120, 0, -80);

    // Right hill
    glColor3f(0.0, 0.5, 0.0);
    fillTriangle(0, -80, 200, 120, 400, -80);
}

// --------------------------------------------------
void drawBus()
{
    glPushMatrix();

    glTranslatef(busX, -140, 0);  // road level
    glScalef(1.8, 2.4, 1.0);

    // Body
    glColor3f(1.0, 0.0, 0.0);   // red bus
    fillRectangle(-60, 0, 120, 40);

    // Top part
    glColor3f(0.8, 0.0, 0.0);
    fillRectangle(-50, 40, 100, 20);

    // Windows
    glColor3f(0.6, 0.9, 1.0);
    fillRectangle(-45, 10, 25, 20);
    fillRectangle(-10, 10, 25, 20);
    fillRectangle(25, 10, 25, 20);

    // Wheels
    glColor3f(0.0, 0.0, 0.0);
    fillCircle(-40, -10, 10);
    fillCircle(40, -10, 10);

    glPopMatrix();
}

// --------------------------------------------------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawSun();
    drawHill();


    drawCloud(cloudX1, 200);
    drawCloud(cloudX2, 170);

    drawfootpath();
    drawRoad();

    drawTree(-320,-80);
    drawTree(-250,-80);
    drawTree(-180,-80);


    drawHouse(-150,-60);
    drawTree(-180,-80);
    drawHouse(60,-60);


    drawTree(150,-80);
    drawTree(230,-80);
    drawTree(310,-80);

    drawRobot();
    drawBus();
//drawRobot();
    glFlush();
}

// --------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'd': robotX += 10; break;
    case 'a': robotX -= 10; break;
    case 'w': robotY += 10; break;
    case 's': robotY -= 10; break;

    case 'i': scaleFactor += 0.1f; break;
    case 'o': if(scaleFactor>0.3f) scaleFactor -= 0.1f; break;

    case 'r': headAngle += 5; break;   // rotate left
case 't': headAngle -= 5; break;   // rotate right

    case 27: exit(0);
    }

    glutPostRedisplay();
}

// --------------------------------------------------
void init()
{
    glClearColor(0.53,0.81,0.92,1); // SKY BLUE
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-400,400,-300,300);
}

// --------------------------------------------------
int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000,700);
    glutCreateWindow("Colored Robot Village");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(30,update,0);

    glutMainLoop();
    return 0;
}
