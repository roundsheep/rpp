
void init_glut()
{
	v=rf.get_param
	num=v.count
	rbuf<char*> argv
	for i=0;i<v.count;i++
		argv.push(v[i].cstr)

	@glutInit(&num,argv.begin)
}

mac FL(a) double(a).tofloat

mac D8(a) 
{
	*(&(a)).to<int*>,*((&(a)).to<int*>+1)
}

mac cd_protect
{
	push ebx;
	push esi;
	push edi;
}

mac cd_ret
{
	pop edi;
	pop esi;
	pop ebx;
	pop ebp;
	retn;
}

mac GL_PI 3.1415926

#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GLfloat float

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100

#define GLUT_KEY_F1 0x0001
#define GLUT_KEY_F2 0x0002
#define GLUT_KEY_F3 0x0003
#define GLUT_KEY_F4 0x0004
#define GLUT_KEY_F5 0x0005
#define GLUT_KEY_F6 0x0006
#define GLUT_KEY_F7 0x0007
#define GLUT_KEY_F8 0x0008
#define GLUT_KEY_F9 0x0009
#define GLUT_KEY_F10 0x000A
#define GLUT_KEY_F11 0x000B
#define GLUT_KEY_F12 0x000C
#define GLUT_KEY_LEFT 0x0064
#define GLUT_KEY_UP 0x0065
#define GLUT_KEY_RIGHT 0x0066
#define GLUT_KEY_DOWN 0x0067
#define GLUT_KEY_PAGE_UP 0x0068
#define GLUT_KEY_PAGE_DOWN 0x0069
#define GLUT_KEY_HOME 0x006A
#define GLUT_KEY_END 0x006B
#define GLUT_KEY_INSERT 0x006C

#define GLUT_LEFT_BUTTON 0x0000
#define GLUT_MIDDLE_BUTTON 0x0001
#define GLUT_RIGHT_BUTTON 0x0002
#define GLUT_DOWN 0x0000
#define GLUT_UP 0x0001
#define GLUT_LEFT 0x0000
#define GLUT_ENTERED 0x0001

#define GLUT_RGB 0x0000
#define GLUT_RGBA 0x0000
#define GLUT_INDEX 0x0001
#define GLUT_SINGLE 0x0000
#define GLUT_DOUBLE 0x0002
#define GLUT_ACCUM 0x0004
#define GLUT_ALPHA 0x0008
#define GLUT_DEPTH 0x0010
#define GLUT_STENCIL 0x0020
#define GLUT_MULTISAMPLE 0x0080
#define GLUT_STEREO 0x0100
#define GLUT_LUMINANCE 0x0200

#define GLUT_MENU_NOT_IN_USE 0x0000
#define GLUT_MENU_IN_USE 0x0001
#define GLUT_NOT_VISIBLE 0x0000
#define GLUT_VISIBLE 0x0001
#define GLUT_HIDDEN 0x0000
#define GLUT_FULLY_RETAINED 0x0001
#define GLUT_PARTIALLY_RETAINED 0x0002
#define GLUT_FULLY_COVERED 0x0003

#define GL_LIGHT0 0x4000
#define GL_LIGHT1 0x4001
#define GL_LIGHT2 0x4002
#define GL_LIGHT3 0x4003
#define GL_LIGHT4 0x4004
#define GL_LIGHT5 0x4005
#define GL_LIGHT6 0x4006
#define GL_LIGHT7 0x4007

#define GL_AMBIENT 0x1200
#define GL_DIFFUSE 0x1201
#define GL_SPECULAR 0x1202
#define GL_POSITION 0x1203
#define GL_SPOT_DIRECTION 0x1204
#define GL_SPOT_EXPONENT 0x1205
#define GL_SPOT_CUTOFF 0x1206
#define GL_CONSTANT_ATTENUATION 0x1207
#define GL_LINEAR_ATTENUATION 0x1208
#define GL_QUADRATIC_ATTENUATION 0x1209

#define GL_DEPTH_TEST 0x0B71
#define GL_LIGHTING 0x0B50

#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_CCW 0x0901
#define GL_CULL_FACE 0x0B44
#define GL_LIGHT_MODEL_AMBIENT 0x0B53
#define GL_COLOR_MATERIAL 0x0B57
#define GL_AMBIENT_AND_DIFFUSE 0x1602
#define GL_SHININESS 0x1601