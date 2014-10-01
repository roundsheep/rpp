class xpoint
{
	double x
	double y
}

namespace cc
{
	enum
	{
		c_cc_set_effect_volume=1000,
		c_cc_set_music_volume,
		c_cc_play_effect,
		c_cc_play_music,
		
		c_cc_add_animations,
		c_cc_get_animation,
		c_cc_animate_create,
		
		c_cc_get_key,
		c_cc_set_key,
		c_cc_add_cache,
		c_cc_rand,
		c_cc_set_design_size,
		c_cc_set_display_state,
		c_cc_jump_scene,
		c_cc_touch_call,
		
		c_cc_sprite_create,
		c_cc_bmf_create,
		c_cc_bmf_set,
		c_cc_get_size,
		c_cc_set_tag,
		c_cc_get_tag,
		c_cc_add_child,
		c_cc_remove_from_parent,
		c_cc_set_pos,
		c_cc_get_pos,
		c_cc_set_scale,
		c_cc_run_ac,
		c_cc_stop_ac,
		c_cc_schedule,

		c_cc_ac_move_to,
		c_cc_ac_move_by,
		c_cc_ac_rotate_to,
		c_cc_ac_rotate_by,
		c_cc_ac_scale_to,
		c_cc_ac_scale_by,
		c_cc_ac_delay,
		c_cc_ac_repeat,
		c_cc_ac_repeat_times,
		c_cc_ac_sequence,
		c_cc_ac_spawn,
		c_cc_ac_call,
		c_cc_ac_ease_in,
		c_cc_ac_ease_out,
		c_cc_ac_elastic_in,
		c_cc_ac_elastic_out,

		c_cc_phy_apply_impulse,
		c_cc_phy_get_body,
		c_cc_phy_set_body,
		c_cc_phy_box,
		c_cc_phy_box_edge,
		c_cc_phy_call,
		
		c_cc_label_create,
		c_cc_set_color,
	}

	set_effect_volume(double val)
	{
		sub esp,8
		mov8 [esp],val
		calle c_cc_set_effect_volume
	}

	set_music_volume(double val)
	{
		sub esp,8
		mov8 [esp],val
		calle c_cc_set_music_volume
	}

	play_effect(char* name)
	{
		push name
		calle c_cc_play_effect
	}

	play_effect(rstr name)
	{
		play_effect(name.cstr)
	}

	play_music(char* name)
	{
		push name
		calle c_cc_play_music
	}

	play_music(rstr name)
	{
		play_music(name.cstr)
	}
	
	add_animations(char* file)
	{
		push file
		calle c_cc_add_animations
	}
	
	add_animations(rstr file)
	{
		add_animations(file.cstr)
	}
	
	void* get_animation(char* name)
	{
		sub esp,4
		push name
		calle c_cc_get_animation
		mov s_ret,[esp]
		add esp,4
	}
	
	void* get_animation(rstr name)
	{
		return get_animation(name.cstr)
	}
	
	void* animate_create(void* animation)
	{
		sub esp,4
		push animation
		calle c_cc_animate_create
		mov s_ret,[esp]
		add esp,4
	}

	rstr get_key(char* name)
	{
		rbuf<char> buf(1024)
		void* begin=buf.begin
		push begin
		push name
		calle c_cc_get_key
		return rstr(begin)
	}
	
	rstr get_key(rstr name)
	{
		return get_key(name.cstr)
	}
	
	set_key(char* name,char* str)
	{
		push str
		push name
		calle c_cc_set_key
	}

	set_key(rstr name,rstr str)
	{
		set_key(name.cstr,str.cstr)
	}
	
	add_cache(char* name)
	{
		push name
		calle c_cc_add_cache
	}
	
	add_cache(rstr name)
	{
		add_cache(name.cstr)
	}
	
	int rand(int n)
	{
		sub esp,4
		push n
		calle c_cc_rand
		mov s_ret,[esp]
		add esp,4
	}
	
	set_design_size(double width,double height)
	{
		sub esp,8
		mov8 [esp],height
		sub esp,8
		mov8 [esp],width
		calle c_cc_set_design_size
	}
	
	set_display_state(bool b)
	{
		push b
		calle c_cc_set_display_state
	}

	jump_scene(void* scene)
	{
		push scene
		calle c_cc_jump_scene
	}
	
	touch_call(void* node,void* call_back)
	{
		push call_back
		push node
		calle c_cc_touch_call
	}

	void* sprite_create(char* name)
	{
		sub esp,4
		push name
		calle c_cc_sprite_create
		mov s_ret,[esp]
		add esp,4
	}
	
	void* sprite_create(rstr name)
	{
		return sprite_create(name.cstr)
	}

	void* bmf_create(char* name,char* str)
	{
		sub esp,4
		push str
		push name
		calle c_cc_bmf_create
		mov s_ret,[esp]
		add esp,4
	}

	void* bmf_create(rstr name,rstr str)
	{
		return bmf_create(name.cstr,str.cstr)
	}

	bmf_set(void* bmf,char* str)
	{
		push str
		push bmf
		calle c_cc_bmf_set
	}

	bmf_set(void* bmf,rstr str)
	{
		bmf_set(bmf,str.cstr)
	}
	
	xpoint get_size(void* sprite)
	{
		sub esp,16
		push sprite
		calle c_cc_get_size
		mov8 s_ret,[esp]
		mov8 [ebp+(s_off s_ret+8)],[esp+8]
		add esp,16
	}
	
	set_tag(void* node,int tag)
	{
		push tag
		push node
		calle c_cc_set_tag
	}
	
	int get_tag(void* node)
	{
		sub esp,4
		push node
		calle c_cc_get_tag
		mov s_ret,[esp]
		add esp,4
	}
	
	add_child(void* parent,void* node,int z)
	{
		push z
		push node
		push parent
		calle c_cc_add_child
	}
	
	remove_from_parent(void* node)
	{
		push node
		calle c_cc_remove_from_parent
	}
	
	add_child(void* parent,void* node)
	{
		add_child(parent,node,0)
	}
	
	set_pos(void* node,double x,double y)
	{
		sub esp,8
		mov8 [esp],y
		sub esp,8
		mov8 [esp],x
		push node
		calle c_cc_set_pos
	}

	set_pos(void* node,xpoint pt)
	{
		set_pos(node,pt.x,pt.y)
	}
	
	xpoint get_pos(void* node)
	{
		sub esp,16
		push node
		calle c_cc_get_pos
		mov8 s_ret,[esp]
		mov8 [ebp+(s_off s_ret+8)],[esp+8]
		add esp,16
	}

	set_scale(void* node,double x,double y)
	{
		sub esp,8
		mov8 [esp],y
		sub esp,8
		mov8 [esp],x
		push node
		calle c_cc_set_scale
	}

	set_scale(void* node,double scale)
	{
		set_scale(node,scale,scale)
	}

	run_ac(void* node,void* action)
	{
		push action
		push node 
		calle c_cc_run_ac
	}
	
	stop_ac(void* node)
	{
		push node
		calle c_cc_stop_ac
	}
	
	schedule(void* node,double time)
	{
		sub esp,8
		mov8 [esp],time
		push node
		calle c_cc_schedule
	}
	
	bool rect_hit(void* a,void* b)
	{
		x1=cc.get_pos(a).x-cc.get_size(a).x/2.0
		x2=cc.get_pos(a).x+cc.get_size(a).x/2.0
		x3=cc.get_pos(b).x-cc.get_size(b).x/2.0
		x4=cc.get_pos(b).x+cc.get_size(b).x/2.0
	
		y1=cc.get_pos(a).y-cc.get_size(a).y/2.0
		y2=cc.get_pos(a).y+cc.get_size(a).y/2.0
		y3=cc.get_pos(b).y-cc.get_size(b).y/2.0
		y4=cc.get_pos(b).y+cc.get_size(b).y/2.0
		
		return rect_hit(x1,x2,x3,x4)&&rect_hit(y1,y2,y3,y4)
	}
	
	bool rect_hit(double a,double b,double c,double d)
	{
		return b>c&&a<d
	}
	
	void* label_create(char* str)
	{
		return label_create(str,"SimSun",30)
	}
	
	void* label_create(char* str,char* font,int size)
	{
		sub esp,4
		push size
		push font
		push str
		calle c_cc_label_create
		mov s_ret,[esp]
		add esp,4
	}
	
	void set_color(void* node,int r,int g,int b)
	{
		push b
		push g
		push r
		push node
		calle c_cc_set_color
	}
}

namespace cca
{
	void* move_to(double time,double x,double y)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],y
		sub esp,8
		mov8 [esp],x
		sub esp,8
		mov8 [esp],time
		calle cc.c_cc_ac_move_to
		mov s_ret,[esp]
		add esp,4
	}
	
	void* move_by(double time,double x,double y)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],y
		sub esp,8
		mov8 [esp],x
		sub esp,8
		mov8 [esp],time
		calle cc.c_cc_ac_move_by
		mov s_ret,[esp]
		add esp,4
	}
	
	void* rotate_to(double time,double angle)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],angle
		sub esp,8
		mov8 [esp],time
		calle cc.c_cc_ac_rotate_to
		mov s_ret,[esp]
		add esp,4
	}
	
	void* rotate_by(double time,double angle)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],angle
		sub esp,8
		mov8 [esp],time
		calle cc.c_cc_ac_rotate_by
		mov s_ret,[esp]
		add esp,4
	}

	void* scale_to(double time,double x,double y)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],y
		sub esp,8
		mov8 [esp],x
		sub esp,8
		mov8 [esp],time
		calle cc.c_cc_ac_scale_to
		mov s_ret,[esp]
		add esp,4
	}

	void* scale_by(double time,double x,double y)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],y
		sub esp,8
		mov8 [esp],x
		sub esp,8
		mov8 [esp],time
		calle cc.c_cc_ac_scale_by
		mov s_ret,[esp]
		add esp,4
	}
	
	void* delay(double time)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],time
		calle cc.c_cc_ac_delay
		mov s_ret,[esp]
		add esp,4
	}
	
	void* repeat(void* action)
	{
		sub esp,4
		push action
		calle cc.c_cc_ac_repeat
		mov s_ret,[esp]
		add esp,4
	}
	
	void* repeat_times(void* action,int times)
	{
		sub esp,4
		push times
		push action
		calle cc.c_cc_ac_repeat_times
		mov s_ret,[esp]
		add esp,4
	}
	
	void* sequence(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		push 2
		calle cc.c_cc_ac_sequence
		mov s_ret,[esp]
		add esp,4
	}
	
	void* sequence(void* a,void* b,void* c)
	{
		sub esp,4
		push c
		push b
		push a
		push 3
		calle cc.c_cc_ac_sequence
		mov s_ret,[esp]
		add esp,4
	}
	
	void* spawn(void* a,void* b)
	{
		sub esp,4
		push b
		push a
		push 2
		calle cc.c_cc_ac_spawn
		mov s_ret,[esp]
		add esp,4
	}
	
	void* call_func(void* call_back)
	{
		sub esp,4
		push call_back
		calle cc.c_cc_ac_call
		mov s_ret,[esp]
		add esp,4
	}
	
	void* schedule(double time,void* call_back)
	{
		return repeat(sequence(delay(time),call_func(call_back)));
	}

	void* ease_in(void* action,double rate)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],rate
		push action
		calle cc.c_cc_ac_ease_in
		mov s_ret,[esp]
		add esp,4
	}

	void* ease_out(void* action,double rate)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],rate
		push action
		calle cc.c_cc_ac_ease_out
		mov s_ret,[esp]
		add esp,4
	}

	void* elastic_in(void* action,double rate)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],rate
		push action
		calle cc.c_cc_ac_elastic_in
		mov s_ret,[esp]
		add esp,4
	}

	void* elastic_out(void* action,double rate)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],rate
		push action
		calle cc.c_cc_ac_elastic_out
		mov s_ret,[esp]
		add esp,4
	}
}

namespace ccp
{
	apply_impulse(void* sprite,double x,double y)
	{
		sub esp,8
		mov8 [esp],y
		sub esp,8
		mov8 [esp],x
		push sprite
		calle cc.c_cc_phy_apply_impulse
	}
	
	void* get_body(void* sprite)
	{
		sub esp,4
		push sprite
		calle cc.c_cc_phy_get_body
		mov s_ret,[esp]
		add esp,4
	}
	
	set_body(void* sprite,void* body)
	{
		push body
		push sprite
		calle cc.c_cc_phy_set_body
	}
	
	void* box(double width,double height)
	{
		sub esp,4
		sub esp,8
		mov8 [esp],height
		sub esp,8
		mov8 [esp],width
		calle cc.c_cc_phy_box
		mov s_ret,[esp]
		add esp,4
	}
	
	void* box_edge(double width,double height,int edge)
	{
		sub esp,4
		push edge
		sub esp,8
		mov8 [esp],height
		sub esp,8
		mov8 [esp],width
		calle cc.c_cc_phy_box_edge
		mov s_ret,[esp]
		add esp,4
	}
	
	hit_call(void* call_back)
	{
		push call_back
		calle cc.c_cc_phy_call
	}
}
