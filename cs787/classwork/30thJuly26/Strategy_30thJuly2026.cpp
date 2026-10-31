// 23B1023
// TextProcessor, 

#include <bits/stdc++.h>
using namespace std;

class TextProcessor {
    public:
        virtual ~TextProcessor() = default;
        virtual string process(string s) = 0;
};

class Capitalize : public TextProcessor {
    public:
        string process(string s) override {
            string ans = "";
            for(int i=0; i<s.length(); i++) {
                if(s[i]>='a' && s[i]<='z')
                    ans += (s[i]-'a'+'A');
                else
                    ans += (s[i]);
            }
            return ans;
        }
};
class makeLower : public TextProcessor {
    public: 
        string process(string s) override {
            string ans = "";
            for(int i=0; i<s.length(); i++) {
                if(s[i]>='A' && s[i]<='Z')
                    ans += (s[i]+'a'-'A');
                else
                    ans += (s[i]);
            }
            return ans;
        }
};
class Reverse : public TextProcessor {
    public:
        string process(string s) override {
            string ans = "";
            for(int i=s.length()-1; i>=0; i--) {
                ans += s[i];
            }
            return ans;
        }
};

int main() {
    TextProcessor *tp;
    string s1, s2;
    s1 = "puShpEndra";
    
    tp = new Capitalize();
    s2 = tp->process(s1);
    cout << s1 << " became " << s2 << endl;

    tp = new makeLower();
    s2 = tp->process(s1);
    cout << s1 << " became " << s2 << endl;

    tp = new Reverse();
    s2 = tp->process(s1);
    cout << s1 << " became " << s2 << endl;
}