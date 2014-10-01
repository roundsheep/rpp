/*
彩色版控制台贪食蛇
该游戏只能以解释模式或JIT模式运行
*/

int g_std_out
rbuf<int> g_arr
int g_next
int g_food

void main()
{
begin:
	init	
	start=rf.tick
	for
		key
		if rf.tick-start<100
			continue
		start=rf.tick
		ifn check
			goto begin
		g_arr.push_front(g_arr[0]+g_next)
		if g_food!=g_arr[0]
			g_arr.pop
		else
			food
		update
}

void update()
{
	for i=0 to 199
		gotoxy(i%10*2,i/10)
		if g_arr.exist(i)
			out("■")
		elif i==g_food
			out("★")
		else
			puts("  ")
}

void init()
{
	rf.srand
	g_std_out=rf.GetStdHandle(-11)
	g_next=10
	g_arr.clear
	g_arr.push(105)
	food

	CONSOLE_CURSOR_INFO cur_info
	cur_info.dwSize=1
	cur_info.bVisible=0
	rf.SetConsoleCursorInfo(g_std_out,&cur_info)
	rf.cmd("mode con cols=20 lines=22")//保留一行留给输入法

	rf.SetConsoleTextAttribute(g_std_out,0x0a)
	gotoxy(0,20)
	puts(" ******************")	
}

void key()
{
	if rf.GetAsyncKeyState(0x26)
		temp=-10
	elif rf.GetAsyncKeyState(0x28)
		temp=10
	elif rf.GetAsyncKeyState(0x25)
		temp=-1
	elif rf.GetAsyncKeyState(0x27)
		temp=1
	else
		return
	if g_arr.count<2||g_arr.get(1)!=g_arr.get(0)+temp
		g_next=temp
}

bool check()
{
	temp=g_arr[0]+g_next
	if temp<0||temp>199||int.abs(temp%10-g_arr[0]%10)>1||g_arr.exist(temp)
		rf.MessageBoxA(0,('over len '+g_arr.count).cstr,"",0)
		return false
	return true
}

void food()
{
	g_food=rf.rand()%200
	if g_arr.exist(g_food)
		food
}

void out(char* s)
{
	puts(rcode.utf8_to_gbk(s))
}

void gotoxy(int x,int y)
{
	COORD pos
	pos.x=x
	pos.y=y
	rf.SetConsoleCursorPosition(g_std_out,*(&pos).to<int*>)
}

struct CONSOLE_CURSOR_INFO
{
	int dwSize
	bool bVisible
}

struct COORD
{
	ushort x
	ushort y
}