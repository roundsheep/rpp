/*
RPP for OpenGL
使用上下左右移动，代码摘自《OpenGL超级宝典》
该例子只能以JIT模式运行，并且需要最新的显卡驱动
运行命令：rjit gl\sphere.h
*/

import math.h
import gl.h
import frame.h

M3DMatrix44f g_shadow_matrix
GLFrame g_camera
double g_y
carray<float,4> g_light_pos
carray<float,4> g_no_light
carray<float,4> g_low_light
carray<float,4> g_bright_light

void main()
{
	g_y=0
	g_light_pos=array<float>[-100.0f, 100.0f, 50.0f, 1.0f]
	g_no_light=array<float>[0.0f, 0.0f, 0.0f, 0.0f]
	g_low_light=array<float>[0.25f, 0.25f, 0.25f, 1.0f]
	g_bright_light=array<float>[1.0f, 1.0f, 1.0f, 1.0f]
	
	void* handle=@LoadLibraryA("gl/freeglut.dll")
	if handle==null
		return
	void[rf.find_dll("set_dll_list")]
	
	init_glut
	@glutInitWindowSize(800,600)
	@glutCreateWindow("sphere")
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

void draw(int shadow)
{
	if shadow==0
		g_y+=0.3f
	else
		@glColor3f(0.0f,0.0f,0.0f)
	if shadow==0
		@glColor3f(0.0f,1.0f,0.0f)
	@glPushMatrix()
	@glTranslatef(0.0f,0.1f,-2.5f)
	if shadow==0
		@glColor3f(0.0f,0.0f,1.0f)
	
	@glPushMatrix()
	@glRotatef(FL(-g_y*2.0f),0.0f,1.0f,0.0f)
	@glTranslatef(1.0f,0.0f,0.0f)
	@glutWireTorus(D8(0.1),D8(0.1),17,9)
	@glPopMatrix()
	
	if shadow==0
		@glColor3f(0.0f,1.0f,0.0f)
		@glMaterialfv(GL_FRONT,GL_SPECULAR,&g_bright_light)
	@glRotatef(g_y.tofloat,0.0f,1.0f,0.0f)
	@glutWireTorus(D8(0.15),D8(0.35),61,37)
	@glMaterialfv(GL_FRONT,GL_SPECULAR,&g_no_light)
	@glPopMatrix()
}

cfunc void render()
{
	@glClear(ORM[GL_COLOR_BUFFER_BIT,GL_DEPTH_BUFFER_BIT])
	@glPushMatrix()
	g_camera.ApplyCameraTransform()
	
	@glLightfv(GL_LIGHT0,GL_POSITION,&g_light_pos)
	
	@glColor3f(0.60f,0.40f,0.10f)
	
	@glDisable(GL_DEPTH_TEST)
	@glDisable(GL_LIGHTING)
	@glPushMatrix()
	@glMultMatrixf(&g_shadow_matrix)
	
	draw(1)
	
	@glPopMatrix()
	@glEnable(GL_LIGHTING)
	@glEnable(GL_DEPTH_TEST)
	
	draw(0)
	@glPopMatrix()
	@glutSwapBuffers()
	@glutPostRedisplay()
}

void input(int key,int x,int y)
{
	switch key
	case GLUT_KEY_UP
		g_camera.MoveForward(0.1f)
	case GLUT_KEY_DOWN
		g_camera.MoveForward(-0.1f)
	case GLUT_KEY_LEFT
		g_camera.MoveRight(0.1f)
	case GLUT_KEY_RIGHT
		g_camera.MoveRight(-0.1f)
	@glutPostRedisplay()
}

void change(int w,int h)
{
	if h==0
		h=1
	@glViewport(0,0,w,h)
	@glMatrixMode(GL_PROJECTION)
	@glLoadIdentity()
	@gluPerspective(D8(35.0),D8(double(w)/h),D8(1.0f),D8(50.0f))
	@glMatrixMode(GL_MODELVIEW)
	@glLoadIdentity()
}

void init()
{
	M3DVector3f vPoints0=array<float>[0.0f,-0.4f,0.0f]
	M3DVector3f vPoints1=array<float>[10.0f,-0.4f,0.0f]
	M3DVector3f vPoints2=array<float>[5.0f,-0.4f,-5.0f]

	@glClearColor(g_low_light[0],g_low_light[1],g_low_light[2],g_low_light[3])
	
	@glCullFace(GL_BACK)
	@glFrontFace(GL_CCW)
	@glEnable(GL_CULL_FACE)
	@glEnable(GL_DEPTH_TEST)
	
	@glLightModelfv(GL_LIGHT_MODEL_AMBIENT,&g_no_light)
	@glLightfv(GL_LIGHT0,GL_AMBIENT,&g_low_light)
	@glLightfv(GL_LIGHT0,GL_DIFFUSE,&g_bright_light)
	@glLightfv(GL_LIGHT0,GL_SPECULAR,&g_bright_light)
	@glEnable(GL_LIGHTING)
	@glEnable(GL_LIGHT0)

	M3DVector4f vPlaneEquation
	GLFrame.m3dGetPlaneEquation(vPlaneEquation,vPoints0,vPoints1,vPoints2)

	GLFrame.m3dMakePlanarShadowMatrix(g_shadow_matrix,vPlaneEquation,g_light_pos)

	@glEnable(GL_COLOR_MATERIAL)
	@glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE)
	@glMateriali(GL_FRONT,GL_SHININESS,128)
}