#include <iostream> 
#include <fstream>
#include <vector>
#include <map>
#include <list>

using namespace std;

struct Span{
    char sym;
    int val;
    int left_val;
    int right_val;
};

struct Comp{
	bool operator() (Span left, Span right){
		return left.val > right.val;
	}
};

list<Span> create_table(map<char, int> rep){
    list<Span> l;
    for(const auto& x : rep){
        Span p;
        p.sym = x.first;
        p.val = x.second;
        l.push_back(p);
    }
    l.sort(Comp());
    l.begin()->right_val = l.begin()->val;
    l.begin()->left_val = 0;
    list<Span>::iterator iter1 = l.begin(), iter2;
    iter2 = iter1;
    iter1++;
    for(; iter1 != l.end(); iter1++){
        iter1->left_val = iter2->right_val;
        iter1->right_val = iter1->left_val + iter1->val;
        iter2++;
    }
    return l;
}

int fileSize(const char* add){
	ifstream mySource;
	mySource.open(add, ios_base::binary);
	mySource.seekg(0, ios_base::end);
	int size = mySource.tellg();
	mySource.close();
	return size;
}

size_t hashfunc(string s){ 
	hash<string> hasher;
	ifstream t(s,ios::out | ios::binary);
	string buffer;
	getline(t, buffer, char(-1));
    size_t hash_buf = hasher(buffer);
	t.close();
	return hash_buf;
}

int main(){
    int count = 0;
    ifstream readf("encoded.txt", ios::binary);
	if(!readf){
		cerr << "Cannot open encoded.txt" << "\n";
        return -1;
	}
	ofstream writef("decoded.txt", ios::binary);
	if(!writef){
		cerr << "Cannot open decoded.txt" << "\n";
        return -2;
	}
    
    int x, y;
	char sym;
    int len = 0;
	map<char, int> rep;														
	readf.read((char*) & x, sizeof(x));	
    while (x > 0){
		readf.read((char*) & sym, sizeof(sym));	
		readf.read((char*) & y, sizeof(y));	
        len += y;
		x--;
		rep[sym] = y;
    }

    list<Span> table = create_table(rep);
    list<Span>::iterator j;
    int low = 0, 
        high = 65535, 
        divisor = table.back().right_val; 
    int First_qtr = (high + 1)/4, 
        Half = First_qtr*2, 
        Third_qtr = First_qtr*3;
    int value = (readf.get() << 8) | readf.get();
    char c = readf.get();
    count = 0;
    while(len){
        int freq = ((value - low + 1)*divisor - 1)/(high - low + 1);
        for(j = table.begin(); j->right_val <= freq; j++);
        int lowold = low;
        low = low + (j->left_val)*(high - low + 1)/divisor;
        high = lowold + (j->right_val)*(high - lowold + 1)/divisor - 1;
        for(;;){
            if(high < Half);
            else if(low >= Half){
                low -= Half; 
                high -= Half; 
                value -= Half;
            }
            else if((low >= First_qtr) && (high < Third_qtr)){
                low -= First_qtr; 
                high -= First_qtr; 
                value -= First_qtr;
            } 
            else break;
            low += low; 
            high += high + 1;
            value += value + (((short)c >> (7 - count)) & 1);
            count ++;
            if(count == 8){
                c = readf.get();
                count = 0;
            }
        }
        writef << j->sym;
        len--;
    }
    readf.close();
    writef.close();

    int code_size = fileSize("encoded.txt");
	int decode_size = fileSize("decoded.txt");
	cout << "encoded:" << code_size << "\n";
	cout << "decoded:" << decode_size << "\n";
	cout << "compress:" << int(code_size * 10000.0 /decode_size )/100.0  << "%\n";

	size_t hash1 = hashfunc("text.txt");
	size_t hash3 = hashfunc("decoded.txt");

	if (hash1 == hash3){
		cout << "hash: files are equal\n";
	}
    return 0;
}