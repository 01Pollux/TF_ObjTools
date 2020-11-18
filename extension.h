/**
 * vim: set ts=4 :
 * =============================================================================
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

/**
 * @file extension.h
 * @brief Sample extension code header.
 */

#include "smsdk_ext.h"
#include <basehandle.h>
#include <ISDKHooks.h>
#include <ISDKTools.h>

#include <list>
#include <vector>

struct IHookInfo
{
public:
	int ref;
	int hookid;
	std::vector<IPluginFunction*> pCallbacks;

public:
	IHookInfo(int id, int ref) : hookid(id), ref(ref) { };
	~IHookInfo()
	{
		if (hookid)
		{
			SH_REMOVE_HOOK_ID(hookid);
			hookid = 0;
		}
	}
};

template<typename C>
class IGlobalList
{
	static std::list<C*>* m_List;

	static void TryAlloc()
	{
		if (!m_List) {
			m_List = new std::list<C*>();
		}
	}
	void TryErase()
	{
		if (m_List) 
		{
			m_List->remove(static_cast<C*>(this));

			if (m_List->empty()) 
			{
				delete m_List;
				m_List = nullptr;
			}
		}
	}

public:
	IGlobalList()
	{
		TryAlloc();
		m_List->push_back(static_cast<C*>(this));
	}
	virtual ~IGlobalList()
	{
		TryErase();
	}

	static const std::list<C*>& List()
	{
		TryAlloc();
		return *m_List;
	}
};
template<typename C> std::list<C*>* IGlobalList<C>::m_List = nullptr;

class IGlobalHooks: public IGlobalList<IGlobalHooks>
{
public:
	virtual bool OnLoad(char*, size_t) abstract;
	virtual void OnUnload() { };
};


extern IGameConfig* gconfig;
extern ISDKTools* sdktools;


#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
