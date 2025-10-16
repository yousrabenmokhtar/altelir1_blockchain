#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;


string simpleHash(const string &data) {
    unsigned int hash = 0;
    for (char c : data)
        hash = (hash * 101 + c) % 1000000007; 
    stringstream ss;
    ss << hex << setw(8) << setfill('0') << hash;
    return ss.str();
}

// --- Fonction pour combiner deux hachages ---
string combineHashes(const string &left, const string &right) {
    return simpleHash(left + right);
}

// --- Fonction pour construire un arbre de Merkle ---
string buildMerkleTree(vector<string> &transactions) {
    if (transactions.empty()) {
        cout << "Aucune transaction fournie." << endl;
        return "";
    }

    // Étape 1 : Hachage de chaque transaction
    vector<string> currentLevel;
    for (const string &tx : transactions) {
        currentLevel.push_back(simpleHash(tx));
    }

    // Étape 2 : Construction des niveaux supérieurs
    while (currentLevel.size() > 1) {
        cout << "Niveau (" << currentLevel.size() << " noeuds):" << endl;
        for (const string &h : currentLevel)
            cout << "  " << h << endl;

        if (currentLevel.size() % 2 != 0)
            currentLevel.push_back(currentLevel.back()); // dupliquer le dernier si impair

        vector<string> nextLevel;
        for (size_t i = 0; i < currentLevel.size(); i += 2) {
            string combined = combineHashes(currentLevel[i], currentLevel[i + 1]);
            nextLevel.push_back(combined);
        }
        currentLevel = nextLevel;
        cout << "  " << endl;
    }

    cout << "Racine de Merkle : " << currentLevel[0] << endl;
    return currentLevel[0];
}

// --- Programme principal ---
int main() {
    vector<string> transactions = {
        "tx1: Ali -> yassine : 8 BTC",
        "tx2: yassine -> Charaf: 6 BTC",
        "tx3: Charaf -> khalil : 3 BTC",
        "tx4: khalil -> yousra : 0.5 BTC",
        "tx5: yousra-> salma : 5 BTC",
        "tx6: salma -> ahmed : 2 BTC",
        "tx7: ahmed -> jalil : 1 BTC",

    };

    cout << "=== Arbre de Merkle Basique ===" << endl;
    string merkleRoot = buildMerkleTree(transactions);
    cout << "\nRacine de Merkle finale : " << merkleRoot << endl;

    return 0;
}
