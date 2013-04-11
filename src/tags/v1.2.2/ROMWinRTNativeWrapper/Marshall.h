#include <vector>
#include <map>
#include <hash_map>

using namespace std;
using namespace Platform;
using namespace Windows::Foundation::Collections;

IVector<String^>^	GetIVectorFromVectorStrings(const vector<wstring> &vect);
vector<wstring>		GetVectorFromIVectorStrings(IVector<String^>^ vect);
IMap<String^, IVector<String^>^>^ GetIMapFromMapStrings(const map<wstring, vector<wstring> > &mp);