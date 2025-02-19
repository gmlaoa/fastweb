﻿/*Software License

Copyright(C) 2024[liuyingjie]
License Terms
Usage Rights

Any individual or entity is free to use, copy, and distribute the binary form of this software without modification to the source code, without the need to disclose the source code.
If the source code is modified, the modifications must be open - sourced under the same license.This means that the modifications must be disclosed and accompanied by a copy of this license.
Future Versions Updates
From this version onwards, all future releases will be governed by the terms of the latest version of the license.This license will automatically be nullified and replaced by the new version.
Users must comply with the terms of the new license issued in future releases.
Liability and Disclaimer
This software is provided “as is”, without any express or implied warranties, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non - infringement.In no event shall the author or copyright holder be liable for any claims, damages, or other liabilities, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
Contact Information
If you have any questions, please contact us: 1585346868@qq.com Or visit our website fwlua.com.
*/

#include "core/subscribemanager.h"
#include "core/config.h"
#include "core/app.h"
#include "core/statemanager.h"
#include "module/http/request.h"
#include "module/http/response.h"
#include "net/http_subscribe.h"
fastweb::subscribe_manager::subscribe_manager(fastweb::app* app) :Interface(app)
{
}
fastweb::subscribe_manager::~subscribe_manager()
{
	clear();
}
void fastweb::subscribe_manager::start()
{

	app()->router->other([&](network::http::request* request, network::http::response* response) {
		this->other(request, response);
		});
}
bool fastweb::subscribe_manager::add(const std::string& pattern, const std::string& filepath)
{
	app()->router->subscribe()->add(pattern, filepath, [&](network::http::request* request, network::http::response* response, const std::string& pattern, const std::string& filepath)->bool {
		return this->callback(request, response, pattern, filepath);
	});
	return true;
}
bool fastweb::subscribe_manager::remove(const std::string& pattern)
{
	return app()->router->subscribe()->remove(pattern);
}
bool fastweb::subscribe_manager::exist(const std::string& pattern)
{
	return app()->router->subscribe()->exist(pattern);
}
void fastweb::subscribe_manager::clear()
{
	if(app()->router != nullptr)
		app()->router->subscribe()->clear();
}

bool fastweb::subscribe_manager::callback(network::http::request* request, network::http::response* response, const std::string& pattern, const std::string& filepath)
{
	bool ok_continue = false;
	auto lua = app()->state->get();
	std::string exception_string;
	try
	{
		sol::load_result script = lua->state->load_file(app()->config->website.dir + filepath);
		if (!script.valid()) {
			sol::error err = script;
			throw ylib::exception(err.what());
		}
		module::request m_request(request);
		module::response m_response(response);
		(*lua->state)["fw_response"] = m_response;
		(*lua->state)["fw_request"] = m_request;

		sol::protected_function_result result = script();
		if (!result.valid()) {
			sol::error err = result;
			throw ylib::exception(err.what());
		}
	}
	catch (const std::exception& e)
	{
		exception_string = e.what();
		if (app()->config->website.debug)
			LOG_ERROR("[subscribe][" + request->filepath() + "]: " + e.what());
	}
	lua->state->collect_garbage();
	app()->state->push(lua);

	if (exception_string.empty() == false)
		throw ylib::exception(exception_string);

	return ok_continue;
}

void fastweb::subscribe_manager::other(network::http::request* request, network::http::response* response)
{
	std::string filepath = request->filepath();
	if (filepath.empty())
		filepath = "/";

	

	// 搜寻默认页面
	if (filepath[filepath.length()-1] == '/')
	{
		std::string default_filepath;
		for (size_t i = 0; i < app()->config->website.default_index.size(); i++)
		{
			default_filepath = app()->config->website.dir + filepath + app()->config->website.default_index[i];
			if (ylib::file::exist(default_filepath))
			{
				filepath += app()->config->website.default_index[i];
				break;
			}
		}
	}

	// 文件全路径
	std::string absolute_path = app()->config->website.dir + filepath;
	// 扩展名
	std::string ext = ylib::file::ext(filepath);
	if (ylib::file::exist(absolute_path) == false)
	{
		send_404(response);
	}
	else if(ext == "lua" && app()->config->website.direct_url_mapping)
	{
		callback(request, response, filepath, filepath);
		return;
	}
	response->send_file(absolute_path);
}

void fastweb::subscribe_manager::send_404(network::http::response* response)
{
	std::string default_404 = app()->config->website.dir + "\\" + app()->config->website.default_404;
	if (app()->config->website.default_404 == "" || ylib::file::exist(default_404) == false)
	{
		response->send((std::string)"404 Not Found", 404, "Not Found");
	}
	else
	{
		response->send_file(default_404, -1, 404, "Not Found");
	}
}
