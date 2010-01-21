/*
This file is part of ROM2NET.
Copyright (C) 2009 Eric D. Schmidt

    ROM2NET is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROM2NET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with ROM2NET.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include "ROM2NET.h"
#include "ROMDictionaryAttributeNET.h";

namespace ROM2NET
{
	public ref class ROMDictionaryNET
	{
	//friend class LinearEngineNET;
	public:
		ROMDictionaryNET(ROMTreeNET^ tree, ROMNode^ context);
		virtual ~ROMDictionaryNET(void) {this->!ROMDictionaryNET();}
		!ROMDictionaryNET(void){}

		void LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttributeNET^ GetDictionaryAttr(String^ dictAttrName);
		Dictionary<String^, ROMDictionaryAttributeNET^>^ GetAllDictionaryAttrs() {return m_dict;}
		
	internal:
		ROMTreeNET^ m_tree;
		ROMNode^ m_context;

		Dictionary<String^, ROMDictionaryAttributeNET^>^ m_dict;
	};
}