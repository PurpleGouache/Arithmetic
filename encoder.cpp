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

list<Span> create_table(const map<char, int>& rep){
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

int main(){
    int count = 0;
    ifstream readf("text.txt", ios::binary);
    if(!readf){
		cerr << "Cannot open text.txt" << "\n";
        return -1;
	}
	ofstream writef("encoded.txt", ios::binary);
	if(!writef){
		cerr << "Cannot open encoded.txt" << "\n";
        return -2;
	}
    map<char, int> rep;
    char c;
    while(readf.get(c))
    {
        rep[c]++;
        count++;
    }
    list<Span> table = create_table(rep);
    int tmp = rep.size();
    writef.write((char*)(&tmp), sizeof(tmp));
	for(const auto& x : rep){
        writef.write((char*)(&x.first), sizeof(x.first));
        writef.write((char*)(&x.second), sizeof(x.second));
    }
    readf.clear();
	readf.seekg(0);
    
    int low = 0, 
        high = 65535, 
        i = 0, 
        divisor = table.back().right_val; 
    int First_qtr = (high + 1)/4, 
        Half = First_qtr*2, 
        Third_qtr = First_qtr*3, 
        bits_to_follow = 0;
    char temp = 0;
    count = 0;
    char j;

    while(readf.get(j)){
        i++;
        auto iter1 = table.begin();
        for(; iter1 != table.end() && j != iter1->sym; iter1++);
        if(iter1 == table.end()){
            cout << "The element not found\n" << endl;
            return -3;
        }
        int lowold = low;
        low = lowold + iter1->left_val*(high - lowold + 1)/divisor;
        high = lowold + iter1->right_val*(high - lowold + 1)/divisor - 1;
        
        for(;;){
            if(high < Half){
                count++;
                if(count == 8){
                    count = 0;
                    writef << temp;
                    temp = 0;
                }
                for(; bits_to_follow > 0; bits_to_follow--){
                    temp = temp | (1 << (7 - count));
                    count++;
                    if(count == 8){
                        count = 0;
                        writef << temp;
                        temp = 0;
                    }
                }
            }
            else if(low >= Half){
                temp = temp | (1 << (7 - count));
                count++;
                if(count == 8){
                    count = 0;
                    writef << temp;
                    temp = 0;
                }                
                for(; bits_to_follow > 0; bits_to_follow--){
                    count++;
                    if(count == 8){
                        count = 0;
                        writef << temp;
                        temp = 0;
                    }
                }
                low -= Half;
                high -= Half;
            }
            else if((low >= First_qtr) && (high < Third_qtr)){
                bits_to_follow++;
                low -= First_qtr;
                high -= First_qtr;
            }
            else break;
            low += low;
            high += high + 1;
        }
    }
    writef << temp;
    readf.close();
    writef.close();
    return 0;
}