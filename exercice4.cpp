#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <ctime>
using namespace std;


class Transaction {
public:
    string id;
    string sender;
    string receiver;
    double amount;

    Transaction(string id_, string s, string r, double a)
        : id(id_), sender(s), receiver(r), amount(a) {}

    string toString() const {
        stringstream ss;
        ss << id << ": " << sender << " -> " << receiver << " : " << amount << " BTC";
        return ss.str();
    }
};


string simpleHash(const string &input) {
    unsigned long hash = 5381;
    for (char c : input)
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    stringstream ss;
    ss << hex << setw(16) << setfill('0') << hash;
    return ss.str();
}

string combineHashes(const string &left, const string &right) {
    return simpleHash(left + right);
}

string buildMerkleRoot(vector<Transaction> &transactions) {
    if (transactions.empty()) return "";

    vector<string> currentLevel;
    for (auto &tx : transactions)
        currentLevel.push_back(simpleHash(tx.toString()));

    while (currentLevel.size() > 1) {
        if (currentLevel.size() % 2 != 0)
            currentLevel.push_back(currentLevel.back()); 

        vector<string> nextLevel;
        for (size_t i = 0; i < currentLevel.size(); i += 2)
            nextLevel.push_back(combineHashes(currentLevel[i], currentLevel[i + 1]));

        currentLevel = nextLevel;
    }
    return currentLevel[0];
}


class Block {
public:
    int index;
    string timestamp;
    string previousHash;
    string merkleRoot;
    string hash;
    long long nonce;
    vector<Transaction> transactions;

    Block(int idx, vector<Transaction> txs, string prevHash)
        : index(idx), previousHash(prevHash), transactions(txs), nonce(0) {
        merkleRoot = buildMerkleRoot(transactions);
        timestamp = currentDateTime();
        hash = calculateHash();
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

    string calculateHash() const {
        stringstream ss;
        ss << index << timestamp << previousHash << merkleRoot << nonce;
        return simpleHash(ss.str());
    }

    void mineBlock(int difficulty) {
        string target(difficulty, '0');
        auto start = chrono::high_resolution_clock::now();

        while (hash.substr(0, difficulty) != target) {
            nonce++;
            hash = calculateHash();
        }

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        cout << "Bloc PoW miné : " << hash
             << " (en " << duration.count() << " s)" << endl;
    }
};


class Blockchain {
public:
    vector<Block> chain;

    Blockchain() {
        vector<Transaction> genesisTx = {
            Transaction("tx0", "system", "yousra", 50)
        };
        chain.push_back(Block(0, genesisTx, "0"));
    }

    Block getLastBlock() const {
        return chain.back();
    }

    void addBlockPoW(vector<Transaction> txs, int difficulty) {
        Block newBlock(chain.size(), txs, getLastBlock().hash);
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
    }


    void addBlockPoS(vector<Transaction> txs, const vector<int> &stakes) {
      
        int total = 0;
        for (int s : stakes) total += s;

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, total);
        int r = dis(gen);

        int selectedIndex = 0, sum = 0;
        for (size_t i = 0; i < stakes.size(); ++i) {
            sum += stakes[i];
            if (r <= sum) {
                selectedIndex = i;
                break;
            }
        }

        Block newBlock(chain.size(), txs, getLastBlock().hash);
        auto start = chrono::high_resolution_clock::now();
        newBlock.hash = newBlock.calculateHash();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        cout << "Bloc PoS validé par le participant " << selectedIndex
             << " : " << newBlock.hash
             << " (en " << duration.count() << " s)" << endl;

        chain.push_back(newBlock);
    }

 
    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            const Block &current = chain[i];
            const Block &previous = chain[i - 1];
            if (current.hash != current.calculateHash()) return false;
            if (current.previousHash != previous.hash) return false;
        }
        return true;
    }
};


int main() {
    Blockchain b;

    cout << "\n=== Test Proof of Work ===\n";
    vector<Transaction> txs1 = {
        Transaction("tx1", "yousra", "fatima", 4),
        Transaction("tx2", "fatima", "charaf", 1)
    };
    b.addBlockPoW(txs1, 3);

    vector<Transaction> txs2 = {
        Transaction("tx3", "charaf", "khalil", 2),
        Transaction("tx4", "khalil", "salma", 0.5)
    };
    b.addBlockPoW(txs2, 3);

    cout << "\n=== Test Proof of Stake ===\n";
    vector<int> stakes = {10, 30, 20, 40};

    vector<Transaction> txs3 = {
        Transaction("tx5", "salma", "ahmed", 1),
        Transaction("tx6", "ahmed", "jalil", 0.3)
    };
    b.addBlockPoS(txs3, stakes);

    vector<Transaction> txs4 = {
        Transaction("tx7", "jalil", "yassine", 2),
        Transaction("tx8", "yassine", "nour", 1.5)
    };
    b.addBlockPoS(txs4, stakes);

    cout << "\n=== Vérification de la blockchain ===\n";
    if (b.isChainValid())
        cout << " Blockchain valide " << endl;
    else
        cout << " Blockchain invalide " << endl;

    return 0;
}
