#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace std;


string simpleHash(const string &input) {
    unsigned long hash = 5381;
    for (char c : input)
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    stringstream ss;
    ss << hex << setw(16) << setfill('0') << hash;
    return ss.str();
}

// ------------------------------------------------------------------
// Classe : Bloc
// ------------------------------------------------------------------
class Block {
public:
    int index;
    string data;
    string previousHash;
    string hash;
    long long nonce;
    string timestamp;

    Block(int idx, string d, string prevHash)
        : index(idx), data(d), previousHash(prevHash), nonce(0) {
        timestamp = currentDateTime();
        hash = calculateHash();
    }

    // Calcul du hash du bloc
    string calculateHash() const {
        stringstream ss;
        ss << index << data << previousHash << nonce << timestamp;
        return simpleHash(ss.str());
    }

    // Preuve de travail
    void mineBlock(int difficulty) {
        string target(difficulty, '0'); // ex : "0000"
        cout << "\n⛏️  Minage du bloc " << index << " avec difficulté " << difficulty << "..." << endl;
        auto start = chrono::high_resolution_clock::now();

        while (hash.substr(0, difficulty) != target) {
            nonce++;
            hash = calculateHash();
        }

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        cout << "✅ Bloc miné : " << hash << endl;
        cout << "⏱️  Temps d’exécution : " << duration.count() << " secondes\n";
    }

    static string currentDateTime() {
        time_t now = time(nullptr);
        tm *ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << 1 + ltm->tm_mon << "-"
           << ltm->tm_mday << " "
           << 1 + ltm->tm_hour << ":"
           << 1 + ltm->tm_min << ":"
           << 1 + ltm->tm_sec;
        return ss.str();
    }
};


class Blockchain {
public:
    vector<Block> chain;
    int difficulty;

    Blockchain(int diff) {
        difficulty = diff;
        chain.emplace_back(0, "Bloc de genèse", "0");
    }

    Block getLastBlock() {
        return chain.back();
    }

    void addBlock(string data) {
        Block newBlock(chain.size(), data, getLastBlock().hash);
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
    }

    void display() {
        cout << "\n=== Contenu de la Blockchain ===\n";
        for (const auto &block : chain) {
            cout << "\nIndex: " << block.index
                 << "\nDonnées: " << block.data
                 << "\nHash précédent: " << block.previousHash
                 << "\nHash: " << block.hash
                 << "\nNonce: " << block.nonce
                 << "\nHorodatage: " << block.timestamp
                 << "\n--------------------------------";
        }
        cout << endl;
    }
};


int main() {
    vector<int> niveaux = {3, 4, 5}; // différents niveaux de difficulté

    for (int diff : niveaux) {
   
        cout << "Test avec difficulté = " << diff << endl;
   

        Blockchain b(diff);
        b.addBlock("Ali envoie 7 BTC à Bob");
        b.addBlock("salim envoie 3 BTC à Charlie");

        b.display();
    }

    return 0;
}
