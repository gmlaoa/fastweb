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
#include "config.h"
#include <regex>
#include "core/app.h"
fastweb::config::config(fastweb::app* ptr):Interface(ptr)
{
}
bool fastweb::config::open(const std::string& ini_filepath)
{
	if (ylib::file::exist(ini_filepath) == false)
	{
		LOG_ERROR("not found file: " + ini_filepath);
		return false;
	}
	std::string temp_filepath =  ylib::file::temp_filepath() + ".bak";
	if (ylib::file::copy(ini_filepath, temp_filepath) == false)
	{
		LOG_ERROR("Failed to copy temporary INI configuration file from '" + ini_filepath + "' to '" + temp_filepath + "'.");
		return false;
	}
	std::string src_content = ylib::file::read(temp_filepath);
	// EXE运行目录
	src_content = strutils::replace(src_content, "${current_dir}", strutils::replace(system::current_dir(),'\\','/'));
	// 配置文件目录
	{
		std::string ini_dir = ylib::file::parent_dir(ini_filepath);
		src_content = strutils::replace(strutils::replace(src_content, "${config_dir}", ini_dir), '\\', '/');
	}
	


	ylib::file::write(temp_filepath,src_content);
	if (m_ini.open(temp_filepath))
	{
		auto vars = extractVariableNames(ylib::file::read(temp_filepath));
		for (size_t i = 0; i < vars.size(); i++)
		{
			if (m_ini.read("variable", vars[i]) == "")
			{
				LOG_ERROR("Variable declaration not found or empty content: ${" + vars[i] + "}");
				return false;
			}
			src_content = strutils::replace(src_content, "${" + vars[i] + "}", m_ini.read("variable", vars[i]));
		}
	}
	ylib::file::write(temp_filepath,src_content);
	m_ini.close();
	m_ini.open(temp_filepath);
	cache();
	return true;
}
std::vector<std::string> fastweb::config::lua_lib_files()
{
	std::vector<std::string> results;
	auto luas = ylib::file::traverse(app()->config->website.dir, "(.*\\.lua)");
	for_iter(iter, luas)
	{
		if (iter->second == IS_DIRECTORY)
			continue;
		std::string path = app()->config->website.dir +"/"+ strutils::replace(iter->first, '\\', '/');
		results.push_back(path);
	}
	// 去重
	std::sort(results.begin(), results.end());
	auto newEnd = std::unique(results.begin(), results.end());
	results.erase(newEnd, results.end());

	
	return results;
}
std::vector<std::string> fastweb::config::extractVariableNames(const std::string& text)
{
	std::regex pattern("\\$\\{([^}]+)\\}"); // 使用捕获组提取中间的内容
	std::vector<std::string> results;
	// 使用 std::sregex_iterator 迭代所有匹配项
	auto begin = std::sregex_iterator(text.begin(), text.end(), pattern);
	auto end = std::sregex_iterator();
	for (std::sregex_iterator i = begin; i != end; ++i) {
		std::smatch match = *i;
		results.push_back(match[1]); // 仅添加捕获组的内容
	}
	return results;
}

void fastweb::config::cache()
{

	scripts.module_dir = PATH_EX(m_ini.read("scripts", "module_dir"));
	scripts.lua_cache_size = ylib::stoi(m_ini.read("scripts", "lua_cache_size"));
	scripts.auto_update_sec = ylib::stoi(m_ini.read("scripts", "auto_update_sec"));

	website.dir = PATH_EX(m_ini.read("website","dir"));
#ifdef _WIN32
	//website.dir_utf8 = codec::to_utf8(website.dir);
#endif
	website.default_404 = m_ini.read("website", "default_404");
	website.default_index = strutils::split(m_ini.read("website", "default_index"), ',');
	website.session_dir = PATH_EX(m_ini.read("website", "session_dir"));
	website.session_timeout_sec = ylib::stoi(m_ini.read("website", "session_timeout_sec"));
	website.Initialization_script = m_ini.read("website", "Initialization_script");
	website.debug = m_ini.read("website", "debug") == "1";
	website.domain = strutils::split(m_ini.read("website", "domain"),',');
	website.direct_url_mapping = m_ini.read("website", "direct_url_mapping") == "1";

	log.enable = m_ini.read("log", "enable") == "1";
	log.dir = PATH_EX(m_ini.read("log", "dir"));
	log.name = m_ini.read("log", "name");
	log.sqlite = m_ini.read("log", "sqlite") == "1";
	//log.succ = m_ini.read("log", "succ") == "1";
	//log.info = m_ini.read("log", "info") == "1";
	//log.warn = m_ini.read("log", "warn") == "1";
	//log.error = m_ini.read("log", "error") == "1";


	// website 域名参数
	for(size_t i=0;i< website.domain.size();i++)
	{
		if (website.domain[i].empty())
			continue;

		struct config::domain dm;
		dm.port = ylib::stoi(m_ini.read(website.domain[i], "port"));
		dm.https = m_ini.read(website.domain[i],"https")=="1";
		dm.ssl.pem_key = ylib::file::read(PATH_EX(m_ini.read(website.domain[i], "ssl_key")));
		dm.ssl.pem_cert = ylib::file::read(PATH_EX(m_ini.read(website.domain[i], "ssl_pem")));
		dm.ssl.pem_ca = ylib::file::read(PATH_EX(m_ini.read(website.domain[i], "ssl_ca")));
		dm.ssl.pem_password = m_ini.read(website.domain[i], "ssl_pwd");
		dm.ssl.type = (network::http::ssl_verify_type)ylib::stoi(m_ini.read(website.domain[i], "ssl_ver_type"));
		if (dm.port == 0)
		{
			LOG_ERROR("domain("+website.domain[i] + ") name unavailable, port is 0");
			continue;
		}
		if (dm.https)
		{
			dm.ssl.enable = true;
			if (dm.ssl.pem_key.empty())
			{
				LOG_ERROR("domain(" + website.domain[i] + ") ssl_key file is read as empty, please check the ssl_key");
				continue;
			}
			if (dm.ssl.pem_cert.empty())
			{
				LOG_ERROR("domain(" + website.domain[i] + ") ssl_pem file is read as empty, please check the ssl_pem");
				continue;
			}
			if (dm.ssl.type < 0 || dm.ssl.type > 3)
			{
				LOG_ERROR("domain(" + website.domain[i] + ") ssl_ver_type is not filled in correctly, it should be: 0~3");
				continue;
			}
		}
		domain.emplace(website.domain[i],dm);
	}
}
