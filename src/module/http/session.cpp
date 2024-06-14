﻿// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com

// QQ：1585346868

#include "session.h"
#include "net/http_website.h"
module::session::session(network::http::session* session):m_session(session)
{
}

module::session::~session()
{
}

void module::session::init(module::request& request, const std::string& id)
{
	if (&request == nullptr)
	{
		throw ylib::exception("the `request` parameter of the init function is nil");
	}
	m_session->init((network::http::website*)request.website(),id);
}

std::string module::session::id()
{
	return m_session->id();
}

void module::session::update()
{
	m_session->update();
}

void module::session::set(const std::string& name, const std::string& value)
{
	m_session->set(name, value);
}

std::string module::session::get(const std::string& name)
{
	return m_session->get(name);
}

bool module::session::check()
{
	return m_session->check();
}

void module::session::regist(sol::state* lua)
{
	lua->new_usertype<module::session>("module_session",
		"check", &module::session::check,
		"get", &module::session::get,
		"id", &module::session::id,
		"set", &module::session::set,
		"update", &module::session::update,
		"init", &module::session::init
	);
}
