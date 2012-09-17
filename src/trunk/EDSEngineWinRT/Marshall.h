#include <vector>
#include <map>
#include <hash_map>

using namespace std;
using namespace Platform;
using namespace Windows::Foundation::Collections;

IVector<String^>^ GetIVectorFromVectorStrings(vector<wstring> vect);
IMap<String^, IVector<String^>^>^ GetIMapFromMapStrings(map<wstring, vector<wstring> > mp);
void MarshalIMapStringUInt(IMap<String^, size_t>^ dict, stdext::hash_map<wstring, size_t> &mp);