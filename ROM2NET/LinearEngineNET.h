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
#include "ROMDictionaryNET.h"

#define MAX_RECURSION 1000

namespace ROM2NET
{
	public ref class LinearEngineNET : ROMDictionaryNET
	{
	public:
		LinearEngineNET(ROMTreeNET^ tree, ROMNode^ context, String^ dictionaryTable);
		virtual ~LinearEngineNET(void){}
		!LinearEngineNET(void){}
		void EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues) {EvaluateForAttribute(dictAttrName, newValues, true);}
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue) {EvaluateForAttribute(dictAttrName, newValue, true);}
		void EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents);
		void EvaluateAll();
		List<ROMDictionaryAttributeNET^>^ GetEvalList() {return m_vEvalList;}
		bool DictionaryIsValid();
		

	private:
		void OrderDictionary();
		void EvalSingleSelect(String^ dictAttrName, String^ newValue);
		void EvalMultiSelect(String^ dictAttrName, array<String^>^ newValues);
		void EvalBoolean(String^ dictAttrName, String^ newValue);
		void EvalEdit(String^ dictAttrName, String^ newValue);
		void EvaluateDependencies(String^ dictAttrName);
		void ResetValueChanged();
		array<String^>^ ParseOutPrefixes(array<String^>^ values, array<String^>^% valuesWithoutPrefixes); //remove the special character flags from the values
		array<String^>^ GetSelectedValues(ROMDictionaryAttributeNET^ attr);

		List<ROMDictionaryAttributeNET^>^ m_vEvalList;
		Dictionary<String^, List<String^>^>^ m_mapTriggers;
		int m_CurrentRecursion;
		array<ROMDictionaryAttributeNET^>^ m_vEvalListRecursChecker;
		bool m_EvalInternal;

		static String^ INVISPREFIX = "^";
		static String^ DEFAULTPREFIX = "@";
		static String^ DISABLEPREFIX = "#";
	};
}