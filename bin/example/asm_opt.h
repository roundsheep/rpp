/*
汇编优化工具
使汇编代码更具可读性，并提高汇编速度
优化15000行大约需要30秒
*/

import rfile.h

main
{
	//name=getsl
	name='http.asm'
	s=rfile.read_all_n(name)

	s=s.replace('\n','\r\n')
	s=s.replace('dword esp','esp')
	s=s.replace('dword ebp','ebp')
	s=s.replace('dword edi','edi')
	s=s.replace('dword esi','esi')
	s=s.replace('dword ebx','ebx')
	s=s.replace(',ecx',', ecx')
	s=s.replace('ebx,0','ebx , 0')
	s=s.replace('ecx,[','ecx , [')
	s=s.replace('mov dword [ edi ] , esi','mov [ edi ] , esi')
	s=opt_sym(s)
	s=line_del(s)
	s=opt_jmp(s)
	s=line_del(s)
	s=opt_two(s)
	s=line_del(s)
	s=sym_replace(s)
	s=line_del(s)

	rfile.write_all_n(name,s)	
}

rstr line_del(rstr& s)
{
	return s.replace('\r\n\r\n\r\n','\r\n\r\n')
}

//符号替换
rstr sym_replace(rstr s)
{
	rbuf<rstr> arr
	r_split_e<rstr>(s,'\r\n',arr)
	rstr ret
	for i in arr
		temp=arr[i]
		if temp.get_top==`:&&is_symbol(temp.sub(0,temp.count-1))
			ret+=sym_trans(temp.sub(0,temp.count-1))+':'
			ret+='\r\n'
			continue
		if (temp.sub(0,5)=='	jmp '||
			temp.sub(0,6)=='	call '||
			temp.sub(0,5)=='	jnz '||
			temp.sub(0,4)=='	jg '||
			temp.sub(0,4)=='	jl '||
			temp.sub(0,5)=='	jae '||
			temp.sub(0,4)=='	jz ')
			index=temp.find(' ')
			symbol=temp.sub(index+1)
			if is_symbol(symbol)
				ret+=temp.sub(0,index+1)+sym_trans(symbol)
				ret+='\r\n'
				continue
		ret+=temp
		ret+='\r\n'
	return ret
}

rstr sym_trans(rstr s)
{
	rstr ret
	for i in s
		two=s.sub(i,i+2)
		if two=='28'||two=='29'//()
			i++
			ret+='_'
		elif two=='2E'||two=='2C'||two=='5F'//.,_
			i++
			ret+='_'
		elif two=='26'
			i++
			ret+='q'
		elif two=='7E'
			i++
			ret+='d'
		elif two=='7C'
			i++
			ret+='h'
		elif two=='5B'
			i++
			ret+='i'
		elif two=='5D'
			i++
			ret+='j'
		elif two=='3D'//=
			i++
			ret+='a'
		elif two=='2B'
			i++
			ret+='b'
		elif two=='2D'//-
			i++
			ret+='s'
		elif two=='2A'//*
			i++
			ret+='t'
		elif two=='2F'
			i++
			ret+='f'
		elif two=='25'
			i++
			ret+='e'
		elif two=='21'//!
			i++
			ret+='o'
		elif two=='3E'//>
			i++
			ret+='x'
		elif two=='3C'//<
			i++
			ret+='y'
		else
			ret+=s[i]
	return seq_replace(ret)
}

rstr seq_replace(rstr s)
{
	//s=s.replace('__','_')
	//s=s.replace('__','_')
	//s=s.replace('__','_')
	if s.get_top==`_
		s.pop
	if s.get_top==`_
		s.pop
	return s
}

//无引用符号删除
rstr opt_sym(rstr s)
{
	rbuf<rstr> arr
	r_split_e<rstr>(s,'\r\n',arr)
	rstr ret
	for i in arr
		temp=arr[i]
		full=temp.sub(0,temp.count-1)
		if temp.get_top==`:&&is_symbol(full)
			symbol=get_symbol(full)
			ifn symbol.empty
				if get_count(arr,i,symbol,full)==1
					continue
		ret+=temp
		ret+='\r\n'
	return ret
}

//jmp后接一个跳转符号
rstr opt_jmp(rstr s)
{
	rbuf<rstr> arr
	r_split_e<rstr>(s,'\r\n',arr)
	rstr ret
	for i in arr
		temp=arr[i]
		a=arr.get(i+1)
		full=a.sub(0,a.count-1)
		if temp.sub(0,5)=='	jmp '&&temp.sub(5)==full&&a.get_top==`:
			symbol=get_symbol(full)
			ifn symbol.empty
				if get_count(arr,i,symbol,full)==2
					i++
					continue
				else
					continue
		ret+=temp
		ret+='\r\n'
	return ret
}

//连续2个jmp，后一个jmp不可能被执行
rstr opt_two(rstr s)
{
	rbuf<rstr> arr
	r_split_e<rstr>(s,'\r\n',arr)
	rstr ret
	for i in arr
		temp=arr[i]
		a=arr.get(i+1)
		if temp.sub(0,5)=='	jmp '&&a.sub(0,5)=='	jmp '
			ret+=temp
			ret+='\r\n'
			i++
			continue
		ret+=temp
		ret+='\r\n'
	return ret
}

int get_count(rbuf<rstr>& arr,int i,rstr& symbol,rstr& full)
{
	for j=i-1;j>=0;j--
		if arr[j].sub(0,arr[j].count-1)==symbol&&`:==arr[j].get_top
			break
	for k=i+1;k<arr.count;k++
		if arr[k].empty
			break
	return find_count(arr,j,k,full)
}

rstr get_symbol(rstr s)
{
	for i=s.count-1;i>=0;i--
		if s[i]==`_&&s.sub(i+1).is_number
			return s.sub(0,i)
	return ''
}

int find_count(rbuf<rstr>& arr,int left,int right,rstr& symbol)
{
	count=0
	for i=left;i<=right;i++
		count+=find_count(arr[i],symbol)
	return count
}

int find_count(rstr src,rstr dst)
{
	count=0
	for i in src
		if src.sub(i,i+dst.count)==dst
			count++
	return count	
}

bool is_symbol(rstr s)
{
	for i in s
		ifn is_symbol(s[i])
			return false
	return true
}

bool is_symbol(char ch)
{
	return ch>=`A&&ch<=`Z||ch>=`a&&ch<=`z||ch>=`0&&ch<=`9||ch==`_
}
