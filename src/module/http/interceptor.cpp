﻿#include "interceptor.h"
#include "core/app.h"

module::interceptor::interceptor()
{
}

module::interceptor::~interceptor()
{
}

bool module::interceptor::add(const std::string& pattern, const std::string& filepath, sol::this_state ts)
{
	GET_APP;
	return app->interceptor->add(pattern,filepath);
}

bool module::interceptor::remove(const std::string& pattern, sol::this_state ts)
{
	GET_APP;
	return app->interceptor->remove(pattern);
}

bool module::interceptor::exist(const std::string& pattern, sol::this_state ts)
{
	GET_APP;
	return app->interceptor->exist(pattern);
}

void module::interceptor::clear(sol::this_state ts)
{
	GET_APP;
	return app->interceptor->clear();
}

void module::interceptor::regist(sol::state* lua)
{
	lua->new_usertype<module::interceptor>("interceptor",
		"add", &module::interceptor::add,
		"remove", &module::interceptor::remove,
		"exist", &module::interceptor::exist,
		"clear", &module::interceptor::clear
	);
}
