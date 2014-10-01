//C++不支持mixin，只能这么写

static rbool cocos_call(int index,tsh* sh,tvm_t* pvmt,treg& reg)
{
#ifndef __COCOS2D_H__
	return false;
#else
	switch(index)
	{
	case tins::c_cc_set_effect_volume:
		{
			SimpleAudioEngine::getInstance()->setEffectsVolume(v_pto_f8(reg.esp));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_set_music_volume:
		{
			SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(v_pto_f8(reg.esp));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_play_effect:
		{
#ifdef _MSC_VER
			SimpleAudioEngine::getInstance()->playEffect(
				(v_pto_pchar(reg.esp)+string(".wav")).c_str());
#else
			SimpleAudioEngine::getInstance()->playEffect(
				(v_pto_pchar(reg.esp)+string(".ogg")).c_str());
#endif
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_play_music:
		{
			SimpleAudioEngine::getInstance()->playBackgroundMusic(v_pto_pchar(reg.esp));
			reg.esp+=4;
			return true;
		}

	case tins::c_cc_add_animations:
		{
			AnimationCache::getInstance()->addAnimationsWithFile(v_pto_pchar(reg.esp));
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_get_animation:
		{
			v_pto_int(reg.esp+4)=(int)AnimationCache::getInstance()->getAnimation(
				v_pto_pchar(reg.esp));
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_animate_create:
		{
			v_pto_int(reg.esp+4)=(int)Animate::create((Animation*)v_pto_int(reg.esp));
			reg.esp+=4;
			return true;
		}

	case tins::c_cc_get_key:
		{
			rstr str(UserDefault::getInstance()->getStringForKey(v_pto_pchar(reg.esp)).c_str());
			if(str.count()>=1024)
			{
				str=str.sub(0,1023);
			}
			memcpy((void*)v_pto_int(reg.esp+4),str.cstr_t(),str.count()+1);
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_set_key:
		{
			UserDefault::getInstance()->setStringForKey(v_pto_pchar(reg.esp),v_pto_pchar(reg.esp+4));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_add_cache:
		{
			xg::addCache((char*)v_pto_int(reg.esp));
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_rand:
		{
			v_pto_int(reg.esp+4)=xg::rand(v_pto_int(reg.esp));
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_set_design_size:
		{
			auto glview=Director::getInstance()->getOpenGLView();
			glview->setDesignResolutionSize(v_pto_f8(reg.esp),v_pto_f8(reg.esp+8),
				ResolutionPolicy::SHOW_ALL);
			reg.esp+=16;
			return true;
		}
	case tins::c_cc_set_display_state:
		{
			Director::getInstance()->setDisplayStats(v_pto_int(reg.esp));
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_jump_scene:
		{
			uint temp=v_pto_uint(reg.esp);
			treg* preg=&reg;
			xg::switchScene([=](Node* parent){
				rbuf<tasm> vasm;
				zasm::push_asm(vasm,"push",rstr((uint)parent));
				zasm::push_asm(vasm,"call",rstr(temp));
				zasm::push_asm(vasm,"halt");
				ifn(zbin::proc_vasm(*sh,vasm))
					return;
				treg tempreg=*preg;
				tempreg.eip=(uint)vasm.begin();
				ifn(qrun(*sh,pvmt,tempreg))
				{
				}
			});
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_touch_call:
		{
			uint temp=v_pto_uint(reg.esp);
			uint call_back=v_pto_uint(reg.esp+4);
			auto listen=EventListenerTouchOneByOne::create();
			listen->onTouchBegan=[=](Touch* touch,Event* event)->bool{
				rbuf<tasm> vasm;
				Point pt=touch->getLocation();
				zasm::push_double(vasm,pt.y);
				zasm::push_double(vasm,pt.x);
				zasm::push_asm(vasm,"call",rstr(call_back));
				zasm::push_asm(vasm,"halt");
				ifn(zbin::proc_vasm(*sh,vasm))
					return true;
				treg tempreg=reg;
				tempreg.eip=(uint)vasm.begin();
				ifn(qrun(*sh,pvmt,tempreg))
				{
				}
				return true;
			};
			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listen,(Node*)temp);
			reg.esp+=8;
			return true;
		}

	case tins::c_cc_sprite_create:
		{
			auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
				(char*)v_pto_uint(reg.esp));
			if(frame==null)
			{
				v_pto_uint(reg.esp+4)=(uint)Sprite::create((char*)v_pto_uint(reg.esp));
			}
			else
			{
				v_pto_uint(reg.esp+4)=(uint)Sprite::createWithSpriteFrame(frame);
			}
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_bmf_create:
		{
			v_pto_int(reg.esp+8)=(int)xbmf::create(
				v_pto_pchar(reg.esp),v_pto_pchar(reg.esp+4));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_bmf_set:
		{
			((LabelBMFont*)v_pto_int(reg.esp))->setString(v_pto_pchar(reg.esp+4));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_get_size:
		{
			Size size=((Sprite*)v_pto_uint(reg.esp))->getContentSize();
			v_pto_f8(reg.esp+4)=size.width;
			v_pto_f8(reg.esp+12)=size.height;
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_set_tag:
		{
			((Node*)v_pto_int(reg.esp))->setTag(v_pto_int(reg.esp+4));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_get_tag:
		{
			v_pto_int(reg.esp+4)=((Node*)v_pto_int(reg.esp))->getTag();
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_add_child:
		{
			((Node*)v_pto_uint(reg.esp))->addChild((Node*)v_pto_uint(reg.esp+4),
				v_pto_uint(reg.esp+8));
			reg.esp+=12;
			return true;
		}
	case tins::c_cc_remove_from_parent:
		{
			((Node*)v_pto_uint(reg.esp))->removeFromParent();
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_set_pos:
		{
			((Node*)v_pto_uint(reg.esp))->setPosition(v_pto_f8(reg.esp+4),
				v_pto_f8(reg.esp+12));
			reg.esp+=20;
			return true;
		}
	case tins::c_cc_get_pos:
		{
			Point pt=((Node*)v_pto_uint(reg.esp))->getPosition();
			v_pto_f8(reg.esp+4)=pt.x;
			v_pto_f8(reg.esp+12)=pt.y;
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_set_scale:
		{
			((Node*)v_pto_uint(reg.esp))->setScale(v_pto_f8(reg.esp+4),
				v_pto_f8(reg.esp+12));
			reg.esp+=20;
			return true;
		}
	case tins::c_cc_run_ac:
		{
			((Node*)v_pto_uint(reg.esp))->runAction((Action*)v_pto_int(reg.esp+4));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_stop_ac:
		{
			((Node*)v_pto_uint(reg.esp))->stopAllActions();
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_schedule:
		{
			return false;
		}
	case tins::c_cc_ac_move_to:
		{
			v_pto_int(reg.esp+24)=(int)MoveTo::create(v_pto_f8(reg.esp),
				Point(v_pto_f8(reg.esp+8),v_pto_f8(reg.esp+16)));
			reg.esp+=24;
			return true;
		}
	case tins::c_cc_ac_move_by:
		{
			v_pto_int(reg.esp+24)=(int)MoveBy::create(v_pto_f8(reg.esp),
				Point(v_pto_f8(reg.esp+8),v_pto_f8(reg.esp+16)));
			reg.esp+=24;
			return true;
		}
	case tins::c_cc_ac_rotate_to:
		{
			v_pto_int(reg.esp+16)=(int)RotateTo::create(
				v_pto_f8(reg.esp),v_pto_f8(reg.esp+8));
			reg.esp+=16;
			return true;
		}
	case tins::c_cc_ac_rotate_by:
		{
			v_pto_int(reg.esp+16)=(int)RotateBy::create(
				v_pto_f8(reg.esp),v_pto_f8(reg.esp+8));
			reg.esp+=16;
			return true;
		}
	case tins::c_cc_ac_scale_to:
		{
			v_pto_int(reg.esp+24)=(int)ScaleTo::create(
				v_pto_f8(reg.esp),v_pto_f8(reg.esp+8),v_pto_f8(reg.esp+16));
			reg.esp+=24;
			return true;
		}
	case tins::c_cc_ac_scale_by:
		{
			v_pto_int(reg.esp+24)=(int)ScaleBy::create(
				v_pto_f8(reg.esp),v_pto_f8(reg.esp+8),v_pto_f8(reg.esp+16));
			reg.esp+=24;
			return true;
		}
	case tins::c_cc_ac_delay:
		{
			v_pto_int(reg.esp+8)=(int)DelayTime::create(v_pto_f8(reg.esp));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_ac_repeat:
		{
			v_pto_int(reg.esp+4)=(int)RepeatForever::create((ActionInterval*)v_pto_int(reg.esp));
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_ac_repeat_times:
		{
			v_pto_int(reg.esp+8)=(int)Repeat::create((ActionInterval*)v_pto_int(reg.esp),
				v_pto_int(reg.esp+4));
			reg.esp+=8;
			return true;
		}
	case tins::c_cc_ac_sequence:
		{
			int n=v_pto_int(reg.esp);
			Vector<FiniteTimeAction*> arr;
			for(int i=0;i<n;i++)
			{
				arr.pushBack((FiniteTimeAction*)v_pto_int(reg.esp+4+i*4));
			}
			v_pto_int(reg.esp+4+n*4)=(int)Sequence::create(arr);
			reg.esp+=4+n*4;
			return true;
		}
	case tins::c_cc_ac_spawn:
		{
			int n=v_pto_int(reg.esp);
			Vector<FiniteTimeAction*> arr;
			for(int i=0;i<n;i++)
			{
				arr.pushBack((FiniteTimeAction*)v_pto_int(reg.esp+4+i*4));
			}
			v_pto_int(reg.esp+4+n*4)=(int)Spawn::create(arr);
			reg.esp+=4+n*4;
			return true;
		}
	case tins::c_cc_ac_call:
		{
			uint call_back=v_pto_uint(reg.esp);
			v_pto_int(reg.esp+4)=(int)CallFuncN::create([=](Node* pthis){
				rbuf<tasm> vasm;
				zasm::push_asm(vasm,"push",rstr((uint)pthis));
				zasm::push_asm(vasm,"call",rstr(call_back));
				zasm::push_asm(vasm,"halt");
				ifn(zbin::proc_vasm(*sh,vasm))
					return;
				treg tempreg=reg;
				tempreg.eip=(uint)vasm.begin();
				qrun(*sh,pvmt,tempreg);
			});
			reg.esp+=4;
			return true;
		}
	case tins::c_cc_ac_ease_in:
		{
			v_pto_int(reg.esp+12)=(int)EaseIn::create((ActionInterval*)v_pto_int(reg.esp),
				v_pto_f8(reg.esp+4));
			reg.esp+=12;
			return true;
		}
	case tins::c_cc_ac_ease_out:
		{
			v_pto_int(reg.esp+12)=(int)EaseOut::create((ActionInterval*)v_pto_int(reg.esp),
				v_pto_f8(reg.esp+4));
			reg.esp+=12;
			return true;
		}
	case tins::c_cc_ac_elastic_in:
		{
			v_pto_int(reg.esp+12)=(int)EaseElasticIn::create((ActionInterval*)v_pto_int(reg.esp),
				v_pto_f8(reg.esp+4));
			reg.esp+=12;
			return true;
		}
	case tins::c_cc_ac_elastic_out:
		{
			v_pto_int(reg.esp+12)=(int)EaseElasticOut::create((ActionInterval*)v_pto_int(reg.esp),
				v_pto_f8(reg.esp+4));
			reg.esp+=12;
			return true;
		}

	case tins::c_cc_label_create:
		{
			v_pto_int(reg.esp+12)=(int)xlabel::create(
				v_pto_pchar(reg.esp),v_pto_pchar(reg.esp+4),v_pto_int(reg.esp+8));
			reg.esp+=12;
			return true;
		}
	case tins::c_cc_set_color:
		{
			((Node*)v_pto_pchar(reg.esp))->setColor(
				Color3B(v_pto_int(reg.esp+4),v_pto_int(reg.esp+8),v_pto_int(reg.esp+12)));
			reg.esp+=16;
			return true;
		}
	default:
		return false;
	}
#endif
}