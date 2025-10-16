#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>

using namespace std;


string simpleHash(const string &input) {
    unsigned long hash = 5381;
    for (char c : input)
        hash = ((hash << 5) + hash) + c;
    stringstream ss;
    ss << hex << setw(16) << setfill('0') << hash;
    return ss.str();
}


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
        hash = simpleHash(to_string(index) + data + previousHash);
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

    Blockchain() {
        chain.emplace_back(0, "Bloc de genèse", "0");
    }

    Block getLastBlock() { return chain.back(); }

    void addBlockPoW(string data, int difficulty) {
        Block newBlock(chain.size(), data, getLastBlock().hash);

        string target(difficulty, '0');
        auto start = chrono::high_resolution_clock::now();
        while (newBlock.hash.substr(0, difficulty) != target) {
            newBlock.nonce++;
            newBlock.hash = simpleHash(to_string(newBlock.index) + newBlock.data + newBlock.previousHash + to_string(newBlock.nonce));
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        cout << "Bloc PoW miné : " << newBlock.hash << " en " << duration.count() << " secondes\n";
        chain.push_back(newBlock);
    }

    void addBlockPoS(string data, const vector<int> &stakes) {
        // Choix aléatoire pondéré par le stake
        vector<int> cumulative;
        int total = 0;
        for (int s : stakes) {
            total += s;
            cumulative.push_back(total);
        }

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, total);
        int r = dis(gen);

        int selectedIndex = 0;
        for (size_t i = 0; i < cumulative.size(); ++i) {
            if (r <= cumulative[i]) {
                selectedIndex = i;
                break;
            }
        }

        Block newBlock(chain.size(), data + " (validé par participant " + to_string(selectedIndex) + ")", getLastBlock().hash);

        auto start = chrono::high_resolution_clock::now();
     
        newBlock.hash = simpleHash(to_string(newBlock.index) + newBlock.data + newBlock.previousHash);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        cout << "Bloc PoS ajouté : " << newBlock.hash << " par participant " << selectedIndex
             << " en " << duration.count() << " secondes\n";
        chain.push_back(newBlock);
    }
};


int main() {
    Blockchain b;

    cout << "\n=== Test Proof of Work ===\n";
    b.addBlockPoW("YOUSRA envoie 4 BTC à fatima", 3);
    b.addBlockPoW("fatima envoie 2 BTC à chrifa", 3);

    cout << "\n=== Test Proof of Stake ===\n";
    vector<int> stakes = {10, 30, 20, 40}; 
    b.addBlockPoS("chrifa envoie 1 BTC à ali", stakes);
    b.addBlockPoS("ali envoie 3 BTC à khalil", stakes);

    return 0;
}
