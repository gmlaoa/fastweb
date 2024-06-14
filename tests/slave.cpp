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

#include "slave.h"
#include "util/system.h"


slave::slave(const std::string& name,int flag):m_flag(flag)
{
	m_sm = std::make_unique<ylib::sharedmem>(false);
	if (m_sm->create(name, 128 * 2) == false)
	{
		std::cerr << m_sm->last_error() << std::endl;
		abort();
	}
}
slave::~slave()
{
	m_sm->destory();
}
void slave::wait()
{
	while ((*m_sm)[m_flag] == 0)
		system::sleep_msec(10);
}

void slave::write(uchar data)
{
	((char*)m_sm->mem())[m_flag+128] = data;
}
