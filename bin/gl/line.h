/*
RPP for OpenGL
使用上下左右旋转，代码摘自《OpenGL超级宝典》
该例子只能以JIT模式运行
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
	v=rf.get_param
	num=v.count
	rbuf<char*> argv
	for i=0;i<v.count;i++
		argv.push(v[i].cstr)

	@glutInit(&num,argv.begin)
	@glutInitWindowSize(800,600)
	@glutCreateWindow("line")
	@glutInitDisplayMode(ORM[GLUT_DOUBLE,GLUT_RGBA,GLUT_DEPTH])
	@glutDisplayFunc(&render)
	@glutSpecialFunc(&input_c)
	@glutReshapeFunc(&change_c)
	
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
	for angle=0.0;angle<=GL_PI;angle+=GL_PI/20
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
	if key==GLUT_KEY_UP
		g_x-=5.0
	if key==GLUT_KEY_DOWN
		g_x+=5.0
	if key==GLUT_KEY_LEFT
		g_y-=5.0
	if key==GLUT_KEY_RIGHT
		g_y+=5.0
	@glutPostRedisplay()
}

void change(int w,int h)
{
	nRange=100.0
	if h==0
		h=1
	@glViewport(0,0,w,h)
	@glMatrixMode(GL_PROJECTION)
	@glLoadIdentity()
	if w<=h
		@glOrtho(D8(-nRange),D8(nRange),D8(-nRange*h/w),
			D8(nRange*h/w),D8(-nRange),D8(nRange))
	else
		@glOrtho(D8(-nRange*w/h),D8(nRange*w/h),D8(-nRange),
			D8(nRange),D8(-nRange),D8(nRange))
	@glMatrixMode(GL_MODELVIEW)
	@glLoadIdentity()
}

void init()
{
	@glClearColor(0.0f,0.0f,0.0f,0.0f)
	@glColor3f(0.0f,1.0f,0.0f)
}

//回调是cdecl，只能这么写
void input_c(int key,int x,int y)
{
	push ebx
	push esi
	push edi
	input(key,x,y)
	pop edi
	pop esi
	pop ebx
	pop ebp
	retn
}

void change_c(int w,int h)
{
	push ebx
	push esi
	push edi
	change(w,h)
	pop edi
	pop esi
	pop ebx
	pop ebp
	retn
}