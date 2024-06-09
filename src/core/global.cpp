﻿
#include "global.h"
#include "module/basemodule.h"
fastweb::global::global(fastweb::app* app):Interface(app)
{
}
fastweb::global::~global()
{
	clear();
}

void fastweb::global::regist(sol::state* lua)
{
	m_ptrs.lock();
	for_iter(iter, (*m_ptrs.parent()))
	{
		auto im = static_cast<module::base*>(iter->second);
		im->regist_global(iter->first.c_str(), lua);
	}
	m_ptrs.unlock();
}
void* fastweb::global::get_ptr(const std::string& name)
{
	void* result = nullptr;
	m_ptrs.get(name,result);
	return result;
}

bool fastweb::global::set_ptr(const std::string& name, void* value, sol::this_state ts)
{
	sol::state_view lua(ts);
	// INIT中先注册一次，防止被销毁
	{
		lua.registry()[name] = this;
		lua[name] = this;
	}
	return m_ptrs.add(name, value);
}

sol::object fastweb::global::get_str(const std::string& name, sol::this_state s)
{
	std::string value;
	if (m_values.get(name, value))
		return  sol::make_object(s, value);
	return sol::make_object(s, sol::nil);
}

void fastweb::global::set_str(const std::string& name, std::string value)
{
	m_values.set(name, value, true);
}

void fastweb::global::clear()
{
	//for_iter(iter, (*m_ptrs.parent()))
	//{
	//	//auto base = ((module::base*)iter->second);
	//	//base->delete_global();
	//}
	m_ptrs.clear();
	m_values.clear();
}
