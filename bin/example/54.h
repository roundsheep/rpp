/*
50行代码实现正则引擎，参考《代码之美》，可以轻松解决95％的正则问题

c 匹配任意的字母c 
.（句点） 匹配任意的单个字符 
^ 匹配输入字符串的开头 
$ 匹配输入字符串的结尾 
* 匹配前一个字符的零个或者多个出现
*/

main
{
	putsl match('.*txt','2343txt')
	putsl match('t.t','2343txt')
}

bool match(rstr reg,rstr text)
{
	if reg.get(0)==`^
		return match_here(reg.sub(1),text)
	for
		if match_here(reg,text)
			return true
		if text.empty
			return false
		text=text.sub(1)
}

bool match_here(rstr reg,rstr text)
{
	if reg.empty
		return true
	if reg.get(1)==`*
		return match_star(reg.get(0),reg.sub(2),text)
	if reg.get(0)==`$&&reg.sub(1).empty
		return text.empty
	if !text.empty&&(reg.get(0)==`.||reg.get(0)==text.get(0))
		return match_here(reg.sub(1),text.sub(1))
	return false
}

bool match_star(char c,rstr reg,rstr text)
{
	for
		if match_here(reg,text)
			return true
		ifn !text.empty&&(text.get(0)==c||c==`.)
			return false
		text=text.sub(1)
}