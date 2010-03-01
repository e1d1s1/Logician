#include "ROMTree.h"
#include "ROMDictionaryAttribute.h";

#pragma once

using namespace std;

namespace ROM
{
	class ROMDictionary
	{
	friend class LinearEngine;
	public:
		ROMDictionary(ROMTree* tree, Node context);
		virtual ~ROMDictionary(void){}

		void LoadDictionary(wstring dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(wstring dictAttrName);
		map<wstring, ROMDictionaryAttribute>* GetAllDictionaryAttrs() {return &m_dict;}
		
		//ASCII overloadas
		void LoadDictionary(string dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(string dictAttrName);

	private:
		ROMTree *m_tree;
		Node m_context;

		map<wstring, ROMDictionaryAttribute> m_dict;
	};
}