
namespace math
{
	double sin(double a)
	{
		p=&s_ret
		push p
		push [ebp+(s_off a+4)]
		push a
		calle "sin"
		add esp,12
	}
}