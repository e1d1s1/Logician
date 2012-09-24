#include <vector>
#include <map>
#include <hash_map>
#include "KnowledgeBase.h"

using namespace std;
using namespace Platform;
using namespace Windows::Foundation::Collections;

IVector<String^>^ GetIVectorFromVectorStrings(const vector<wstring> &vect);
IMap<String^, IVector<String^>^>^ GetIMapFromMapStrings(const map<wstring, vector<wstring> > &mp);
void MarshalIMapStringUInt(IMap<String^, size_t>^ dict, MAPWSTRUINT &mp);