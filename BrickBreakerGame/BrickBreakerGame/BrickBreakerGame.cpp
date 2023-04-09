#include <GL/freeglut_std.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <string>,
#include <iostream>
#include <SOIL.h>

#define WINDOW_W 1200
#define WINDOW_H 800

static GLfloat paddle_x = -7.5;
static GLfloat paddle_y = -23.0;
static GLfloat bricks_width = 8;
static GLfloat bricks_height = 2;
static GLfloat paddle_size = 15;
static GLfloat paddle_height = 2;
static GLfloat moveXFactor = 0.2;
static GLfloat moveYFactor = 0.5;
static const int FPS = 60;

GLuint texture_heart;
GLuint texture_wall;
GLuint texture_paddle;
GLuint texture_ball;
GLuint texture_backgroundmain;
GLuint texture_backgroundplay;
GLuint texture_startbtn;
GLuint texture_score;
GLuint texture_backgroundendlose;
GLuint texture_quitbtn;
GLuint texture_backgroundendwin;

void startgame();
void endgamemenu();
void playgame();
void drawwallimg(int right, int top);

int mouseX = 0;
int mouseY = 0; 
int temp = 1;
int score = 0;
int bricksnumber = 0;
int lives=5;
int buttonx1 = -10;
int buttonx2 = 8;
int buttony1 = 2;
int buttony2 =8;
int bricks[7][11] = { 0 };


float red = 1.0f;
float green = 0.0f;
float blue = 0.0f;
float color_r, color_g, color_b;

bool isgameStarted = false;
bool isendgame = false;
bool iswin = false;
bool istopstop = true;


struct _ball
{
	GLfloat radius = 1.2;
	GLfloat X = 0.0;
	GLfloat Y = 0.0;
	int directionX = 0;
	int directionY = 0;
}ball;

void init_bricks() {
	bricksnumber = 0;
	for (int row = 0; row < 7; row++) {
		for (int col = 0; col < 11; col++) {
			bricks[row][col] = 1;
			bricksnumber++;
		}
	}
}

void draw_bricks() {


	glClear(GL_COLOR_BUFFER_BIT);

	// Background İmage
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture_backgroundplay);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-51, -33.5);
	glTexCoord2f(1, 0);
	glVertex2f(51, -33.5);
	glTexCoord2f(1, 1);
	glVertex2f(51, 33.5);
	glTexCoord2f(0, 1);
	glVertex2f(-51, 33.5);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// RGB BORDER
	glPushMatrix();
	glColor3f(fabs(sin(color_r)), fabs(sin(color_g)), fabs(sin(color_b)));
	glLineWidth(5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-51, 27);
	glVertex2f(50, 27);
	glVertex2f(50, 27);
	glVertex2f(-51, 27);
	glEnd();
	glPopMatrix();

	color_r += 0.01f;
	color_g += 0.02f;
	color_b += 0.03f;

	//Tuğlaları ekrana yazdırma
	GLfloat top = 24.0;
	GLfloat right = -45.0;
	for (int row = 0; row < 7; row++) {
		for (int col = 0; col < 11; col++) {
			if (bricks[row][col] == 1) {

				//tuğlalara beyaz border ekleme
				glBegin(GL_LINE_LOOP);
				glClear(GL_COLOR_BUFFER_BIT);
				glPushMatrix();
				glColor3f(1.0f, 1.0f, 1.0f);
				glLineWidth(1.0f);
				glVertex2f(right, top);
				glVertex2f(right + bricks_width, top);
				glVertex2f(right + bricks_width, top - bricks_height);
				glVertex2f(right, top - bricks_height);
				glPopMatrix();
				glEnd();

				//tuğlaları yazdırma 
				glBegin(GL_QUADS);
				glColor3f(0.6, 0.3, 0.1);
				glVertex2f(right, top);
				glVertex2f(right + bricks_width, top);
				glVertex2f(right + bricks_width, top - bricks_height);
				glVertex2f(right, top - bricks_height);
				glEnd();

				// tuğlalara resim basma
				drawwallimg(right, top);
			}
			right += bricks_width;
		}
		// tuğlalara şekil verme 
		if (row % 2 == 1) {
			right = -45.0;
		}
		else {
			right = -41.0;
		}
		top -= bricks_height;
	}
}

void check_collision() {
	int right;
	bool collided = false;
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 11; j++) {
			if (bricks[i][j] == 1) {
				if (i % 2 == 1) {
					right = -41.0;
				}
				else {
					right = -45.0;
				}
				GLfloat dist_x = fabs(ball.X - ((right + j * bricks_width) + bricks_width / 2.0));
				GLfloat dist_y = fabs(ball.Y - ((28.0 - i * bricks_height) - bricks_height / 2.0));

				if (dist_x < ball.radius + bricks_width / 2.0 && dist_y < ball.radius + bricks_height / 2.0) {
					bricks[i][j] = 0;
					bricksnumber--;
					score += 10;
					glColor3f(1.0, 1.0, 1.0);
					glBegin(GL_QUADS);
					glVertex2f((right + j * bricks_width), (28.0 - i * bricks_height));
					glVertex2f((right + (j + 1) * bricks_width), (28.0 - i * bricks_height));
					glVertex2f((right + (j + 1) * bricks_width), (28.0 - (i + 1) * bricks_height));
					glVertex2f((right + j * bricks_width), (28.0 - (i + 1) * bricks_height));
					glEnd();

					if (!collided) {
						if (ball.directionY > 0) {
							ball.Y = (28.0 - (i + 1) * bricks_height) - ball.radius - 0.01;
						}
						else {
							ball.Y = (28.0 - i * bricks_height) + ball.radius + 0.01;
						}
						ball.directionY = ball.directionY * (-1);
						collided = true;
					}

					if (bricksnumber == 0 && isgameStarted) {
						isendgame = true;
						iswin = true;
						glutDisplayFunc(endgamemenu);
					}
				}
			}
		}
	}
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0 / aspect, 50.0 / aspect, -1.0, 1.0);
}

void draw_circle(float x, float y, float radius) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	static const int circle_points = 100;
	static const float angle = 2.0f * 3.1416f / circle_points;

	// this code (mostly) copied from question:
	glBegin(GL_POLYGON);
	double angle1 = 0.0;
	glVertex2d(radius * cos(0.0), radius * sin(0.0));
	int i;
	for (i = 0; i < circle_points; i++)
	{
		glVertex2d(radius * cos(angle1), radius * sin(angle1));
		angle1 += angle;
	}
	glEnd();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_ball);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x - radius , y - radius );
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x + radius , y - radius );
	glTexCoord2f(1.0f, 1.0f); glVertex2f(x + radius , y + radius );
	glTexCoord2f(0.0f, 1.0f); glVertex2f(x - radius , y + radius );
	glEnd();
	glDisable(GL_TEXTURE_2D);






}

void draw_paddle(int paddle_x,int paddle_y) {


	


	glPushMatrix();
	glColor3f(0.6, 1, 0.9);
	glRectf(paddle_x, paddle_y, paddle_x + paddle_size, paddle_y - paddle_height);
	

	glEnable(GL_TEXTURE_2D);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture_paddle);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	
	glVertex2f(paddle_x, paddle_y); // Sol alt köşe
	
	glTexCoord2f(1, 1);
	glVertex2f(paddle_x + paddle_size, paddle_y); // Sağ alt köşe
	glTexCoord2f(1, 0);
	glVertex2f(paddle_x + paddle_size, paddle_y - paddle_height); // Sağ üst köşe
	glTexCoord2f(0, 0);
	glVertex2f(paddle_x, paddle_y - paddle_height); // Sol üst köşe
	glEnd();
	glDisable(GL_TEXTURE_2D);


	glPopMatrix();



}

void drawScore() {
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture_score);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-45 , 29); // Sol alt köşe
	glTexCoord2f(1, 0);
	glVertex2f(-33, 29); // Sağ alt köşe
	glTexCoord2f(1, 1);
	glVertex2f(-33 , 32); // Sağ üst köşe
	glTexCoord2f(0, 1);
	glVertex2f(-45, 32); // Sol üst köşe
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
	std::string scoreStr =std::to_string(score);
	glPushMatrix();

	glColor3f(0.8 , 0 , 0.8);
	glRasterPos2f(-39.7, 29.8);

	for (int i = 0; i < scoreStr.length(); i++) {

		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr[i]);


	}
	glPopMatrix();


}

void drawwallimg(int right,int top) {


	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture_wall);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(right, top); // Sol alt köşe
	glTexCoord2f(1, 0);
	glVertex2f(right + bricks_width, top); // Sağ alt köşe
	glTexCoord2f(1, 1);
	glVertex2f(right + bricks_width, top - bricks_height); // Sağ üst köşe
	glTexCoord2f(0, 1);
	glVertex2f(right, top - bricks_height); // Sol üst köşe
	glEnd();
	glDisable(GL_TEXTURE_2D);
	

}

void Drawlivesimg()
{
	int heartx = 0;
	for (int i = 0; i < lives; i++) {

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texture_heart);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(45 - heartx, 29); // Sol alt köşe
		glTexCoord2f(1, 0);
		glVertex2f(42 - heartx, 29); // Sağ alt köşe
		glTexCoord2f(1, 1);
		glVertex2f(42 - heartx, 31); // Sağ üst köşe
		glTexCoord2f(0, 1);
		glVertex2f(45- heartx, 31); // Sol üst köşe
		glEnd();
		glDisable(GL_TEXTURE_2D);
		heartx += 4;
	}
}

void timer(int v)
{
	glutPostRedisplay();
	if (ball.Y < -29.0)
	{
		
		lives--;

		if (lives < 1) {
			
			isendgame = true;
			iswin = false;
			glutDisplayFunc(endgamemenu);

		}

		ball.X = 0;
		ball.Y = -22;
		ball.directionX = 0;
		ball.directionY = 0;
		paddle_x = -7.5;
		istopstop = true;
		

	}
	

	if ((ball.Y < -23) && ball.directionY < 0) {
		if ((paddle_x <= ball.X) && (ball.X < (paddle_x + 5))) {
			ball.directionX = -1;
			ball.directionY = -ball.directionY;
		}
		else if ((paddle_x + 5 <= ball.X) && (ball.X < paddle_x + 10)) {
			ball.directionX = 0;
			ball.directionY = -ball.directionY;
		}
		else if ((paddle_x + 10 <= ball.X) && (ball.X < paddle_x + 15)) {
			ball.directionX = 1;
			ball.directionY = -ball.directionY;
		}
	}
	else if ((ball.Y > 26) && ball.directionY > 0) {
		ball.directionY = -ball.directionY;
	}
	if ((ball.X < -48 && ball.directionX < 0) || (ball.X > 48 && ball.directionX > 0)) {
		ball.directionX = -ball.directionX;
	}
	ball.X += moveXFactor * ball.directionX;
	ball.Y += moveYFactor * ball.directionY;

	
	glutTimerFunc(1000 / FPS, timer, v);


	}

void mouseMove(int x, int y) {
	mouseX = x;
	
	mouseY = y;
}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		
		
		printf("x : %d , y : %d  \n", x, y); 
		if (x >  480  && x < 680  && y > 325 && y < 350 && !isgameStarted && !isendgame) {
			isgameStarted = true;
		}
		
		if (x > 280 && x < 850 && y >565 && y < 630 && isendgame && isgameStarted && !iswin) {
			isendgame = false;
			isgameStarted = false;
			
			glutDisplayFunc(startgame);
		}
		if (x > 440 && x < 770 && y >550 && y < 710 && isendgame && isgameStarted && iswin) {
			isendgame = false;
			isgameStarted = false;

			glutDisplayFunc(startgame);
			
		}


		if (x > 500 && x < 700 && y >680 && y < 750 && isendgame && isgameStarted && !iswin) {
			
			exit(1);
		}
	}
}
	
void kbSpecial(int key, int x, int y)
{
	
	switch (key)
	{
		
			
	case GLUT_KEY_UP:
		
		break;
	case GLUT_KEY_DOWN:
		
		break;
	case GLUT_KEY_LEFT:
		if (paddle_x > -50) {
		paddle_x = paddle_x - 1;
		draw_paddle(paddle_x, paddle_y);
		}
		break;
	case GLUT_KEY_RIGHT:
		
		if (paddle_x < 50-paddle_size) {
			paddle_x = paddle_x + 1;
			draw_paddle(paddle_x, paddle_y);
		}
		break;

	}
}

void keyboardFunc(unsigned char key, int x, int y)
{

	if (key == 32 && isgameStarted && !isendgame && istopstop) {
		
		ball.directionY = 1;
		istopstop = false;
		}	
	}

void loadTextures() {
	texture_wall = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/wall.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_wall == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_heart = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/heart2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_heart == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_paddle = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/paddle.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_heart == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_ball = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/ball.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_ball == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_backgroundmain = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/backgroundmain.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_backgroundmain == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_startbtn = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/start-button.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_startbtn == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_score = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/score.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_score == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_backgroundplay = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/background_play.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_backgroundplay == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_backgroundendlose = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/endgamelose.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_backgroundendlose == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	texture_quitbtn = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/quitbtn.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_quitbtn == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}
	texture_backgroundendwin = SOIL_load_OGL_texture("C:/Users/Durasel/source/repos/BrickBreakerGame/img/endgamewin.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_backgroundendwin == 0) {
		printf("Texture loading failed: '%s'\n", SOIL_last_result());
	}

	
	
	

	

}

void startgame() {

	if (!isgameStarted) { // Oyun henüz başlamadı
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		loadTextures();


		// arka plan resmi yüklendi ilk ekran
		glEnable(GL_BLEND);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texture_backgroundmain);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-51, -33.5);
		glTexCoord2f(1, 0);
		glVertex2f(51, -33.5);
		glTexCoord2f(1, 1);
		glVertex2f(51, 33.5);
		glTexCoord2f(0, 1);
		glVertex2f(-51, 33.5);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glFlush();

		// start buton eklendi
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texture_startbtn);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(buttonx1, buttony1);
		glTexCoord2f(1, 0);
		glVertex2f(buttonx2, buttony1);
		glTexCoord2f(1, 1);
		glVertex2f(buttonx2, buttony2);
		glTexCoord2f(0, 1);
		glVertex2f(buttonx1, buttony2);
		glEnd();
		glDisable(GL_TEXTURE_2D);



	}

	else { // oyun basladi

		
		init_bricks();
		draw_bricks();

		ball.X = 0;
		ball.Y = -22;
		ball.directionX = 0;
		ball.directionY = 0;
		score = 0;
		lives = 5;
		istopstop = true;

		glutDisplayFunc(playgame);
		if (temp == 1) {
		glutTimerFunc(100, timer, 0);
		temp--;
		}

	}

	glutSwapBuffers();


	
}

void playgame()

{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	check_collision();
	draw_bricks();
	drawScore();
	Drawlivesimg();
	draw_paddle(paddle_x, paddle_y);
	draw_circle(ball.X, ball.Y, ball.radius);
	glFlush();
	glutSwapBuffers();
}

void endgamemenu() {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	if (iswin) {

		glEnable(GL_BLEND);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texture_backgroundendwin);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-51, -33.5);
		glTexCoord2f(1, 0);
		glVertex2f(51, -33.5);
		glTexCoord2f(1, 1);
		glVertex2f(51, 33.5);
		glTexCoord2f(0, 1);
		glVertex2f(-51, 33.5);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		
		
	}

	else {


		glEnable(GL_BLEND);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texture_backgroundendlose);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-51, -33.5);
		glTexCoord2f(1, 0);
		glVertex2f(51, -33.5);
		glTexCoord2f(1, 1);
		glVertex2f(51, 33.5);
		glTexCoord2f(0, 1);
		glVertex2f(-51, 33.5);
		glEnd();
		glDisable(GL_TEXTURE_2D);


		//quit button

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_quitbtn);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-10 , -30);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(10 , -30 );
		glTexCoord2f(1.0f, 1.0f); glVertex2f(10, -23 );
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-10 , -23);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		
	}
		                   						
		ball.X = 0;
		ball.Y = -22;
		ball.directionX = 0;
		ball.directionY = 0;
		score = 0;
		lives = 5;
		paddle_x = -7.5;
		paddle_y = -23.0;
		
		
		glFlush();
		glutSwapBuffers();

	
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(260, 140);
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutCreateWindow("Brick Breaker Game");
	glutReshapeFunc(reshape);
	glutDisplayFunc(startgame);
	glutMotionFunc(mouseMove);
	glutMouseFunc(mouseButton);
	glutSpecialFunc(kbSpecial);
	glutKeyboardFunc(keyboardFunc);
	
	glutMainLoop();
}