#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

string findsh(const string& word1, const string& word2) {
    int len1 = word1.length();
    int len2 = word2.length();
    string shortest = "";
    int minLength = 100000;
    for (int i = 0; i < len1; i++) {
        for (int j = i; j < len1; j++) {
            string substring = word1.substr(i, j - i + 1);
            if (word2.find(substring) != string::npos && substring.length() < minLength) {
                minLength = substring.length();
                shortest = substring;
            }
        }
    }
    return shortest;
}

int main() {
	string line;
	cout << "Vvedite stroku iz 10 slov: ";
	getline(cin, line);

	vector<string> words;
    stringstream ss(line);
    string word;

    while (ss >> word) {
        words.push_back(word);
    }
    if (words.size() != 10) {
        cout << "Nado 10 slov!" << endl;
        return 1;
    }

    for (string& wd : words) {
        transform(wd.begin(), wd.end(), wd.begin(), [](unsigned char c) { return tolower(c); });
    }

    int slovaA = 0;
    int minline = 100000;
    int az = 0;
    int j = 0;

    for (int i = 0; i < 10; i++) {
        if (words[i][0] == 'a' || words[i][0] == 'A')
            slovaA++;
        if (words[i].length() < minline)
            minline = words[i].length();
        j = words[i].length() - 1;
        if (words[i][0] == words[i][j])
            az++;
    }

    int a10 = 0;
    for (int k = 0; k < words[9].length(); k++) {
        if (words[9][k] == 'a')
            a10++;
    }

    cout << "Kol-vo slov na 'a': " << slovaA << endl;
    cout << "Dlina min slova: " << minline << endl;
    cout << "Kol-vo 'a' v 10 slove: " << a10 << endl;
    for (int i = 0; i < 10; i++) {
        cout << words[i];
    }
    cout << endl;
    cout << "Kol-vo slov gde 1 i posledniy sovpadaut: " << az << endl;
    
    int minnn = 100000;
    string minl = "";
    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (findsh(words[i], words[j]).length() < minnn && !findsh(words[i], words[j]).empty()) {
                minnn = findsh(words[i], words[j]).length();
                minl = findsh(words[i], words[j]);
            }
        }
    }
    if (!minl.empty())
        cout << "Podstroka: " << minl;
    else
        cout << "Podstroki net";

	return 0;
}