/*
RPP for OpenGL
使用上下左右旋转，代码摘自《OpenGL超级宝典》
该例子只能以JIT模式运行，并且需要最新的显卡驱动
运行命令：rjit gl\line.h
*/

import math.h
import gl.h

double g_x
double g_y

void main()
{
	g_x=0
	g_y=0
	
	void* handle=@LoadLibraryA("gl/freeglut.dll")
	if handle==null
		return
	void[rf.find_dll("set_dll_list")]
	
	init_glut
	@glutInitWindowSize(800,600)
	@glutCreateWindow("line")
	@glutInitDisplayMode(ORM[GLUT_DOUBLE,GLUT_RGBA,GLUT_DEPTH])
	@glutDisplayFunc(&render)
	
	@glutSpecialFunc(lambda(int key,int x,int y){
		cd_protect
		input(key,x,y)
		cd_ret
	})
	@glutReshapeFunc(lambda(int w,int h){
		cd_protect
		change(w,h)
		cd_ret
	})
	
	init()
	
	@glutMainLoop()
	
	@FreeLibrary(handle)
}

cfunc void render()
{
	@glClear(GL_COLOR_BUFFER_BIT)
	@glPushMatrix()
	@glRotatef(FL(g_x),1.0f,0.0f,0.0f)
	@glRotatef(FL(g_y),0.0f,1.0f,0.0f)

	@glBegin(GL_LINES)
	z=0.0
	for angle=0.0;angle<=GL_PI;angle+=GL_PI/20.0
		x=50.0*math.sin(angle)
		y=50.0*math.cos(angle)
		@glVertex3f(FL(x),FL(y),FL(z))
		x=50.0*math.sin(angle+GL_PI)
		y=50.0*math.cos(angle+GL_PI)	
		@glVertex3f(FL(x),FL(y),FL(z))
	@glEnd()
	
	@glPopMatrix()
	@glutSwapBuffers()
}

void input(int key,int x,int y)
{
	switch key
	case GLUT_KEY_UP
		g_x-=5.0
	case GLUT_KEY_DOWN
		g_x+=5.0
	case GLUT_KEY_LEFT
		g_y-=5.0
	case GLUT_KEY_RIGHT
		g_y+=5.0
	@glutPostRedisplay()
}

void change(int w,int h)
{
	range=100.0
	if h==0
		h=1
	@glViewport(0,0,w,h)
	@glMatrixMode(GL_PROJECTION)
	@glLoadIdentity()
	if w<=h
		@glOrtho(D8(-range),D8(range),D8(-range*h/w),
			D8(range*h/w),D8(-range),D8(range))
	else
		@glOrtho(D8(-range*w/h),D8(range*w/h),D8(-range),
			D8(range),D8(-range),D8(range))
	@glMatrixMode(GL_MODELVIEW)
	@glLoadIdentity()
}

void init()
{
	@glClearColor(0.0f,0.0f,0.0f,0.0f)
	@glColor3f(0.0f,1.0f,0.0f)
}