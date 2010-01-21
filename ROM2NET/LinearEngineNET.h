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
		

	private:
		void OrderDictionary();
		void EvalSingleSelect(String^ dictAttrName, String^ newValue);
		void EvalMultiSelect(String^ dictAttrName, array<String^>^ newValues);
		void EvalBoolean(String^ dictAttrName, String^ newValue);
		void EvalEdit(String^ dictAttrName, String^ newValue);
		void EvaluateDependencies(String^ dictAttrName);
		array<String^>^ ParseOutPrefixes(array<String^>^ values, array<String^>^% valuesWithoutPrefixes); //remove the special character flags from the values
		array<String^>^ GetSelectedValues(ROMDictionaryAttributeNET^ attr);

		List<ROMDictionaryAttributeNET^>^ m_vEvalList;
		Dictionary<String^, List<String^>^>^ m_mapTriggers;
		int m_CurrentRecursion;

		static String^ INVISPREFIX = "^";
		static String^ DEFAULTPREFIX = "@";
		static String^ DISABLEPREFIX = "#";
	};
}