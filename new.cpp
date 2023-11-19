#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <cctype>
#include <memory>

using namespace std;

// Node structure for QTree
struct Node {
    char symbol;
    unique_ptr<Node> left;
    unique_ptr<Node> right;
};

// Comparator for priority queue
struct CompareNodes {
    bool operator()(const Node* a, const Node* b) const {
        return a->symbol > b->symbol;
    }
};

// Function to delete nodes in the tree
void deleteTree(Node* node) {
    if (node) {
        deleteTree(node->left.get());
        deleteTree(node->right.get());
    }
}

// Function to construct QTree from frequency data
unique_ptr<Node> constructQTree(const unordered_map<char, int>& frequency) {
    priority_queue<Node*, vector<Node*>, CompareNodes> pq;

    // Create leaf nodes for each character and add them to the priority queue
    for (const auto& pair : frequency) {
        unique_ptr<Node> leaf = make_unique<Node>();
        leaf->symbol = pair.first;
        leaf->left = nullptr;
        leaf->right = nullptr;
        pq.push(leaf.release());
    }

    // Construct the QTree by merging nodes
    while (pq.size() > 1) {
        unique_ptr<Node> left(pq.top());
        pq.pop();

        unique_ptr<Node> right(pq.top());
        pq.pop();

        unique_ptr<Node> parent = make_unique<Node>();
        parent->symbol = 0;  // Internal node symbol is not relevant
        parent->left = move(left);
        parent->right = move(right);

        pq.push(parent.release());
    }

    // Return the root of the QTree
    return unique_ptr<Node>(pq.top());
}

// Recursive function to generate bit-trails
void generateBitTrails(Node* node, string trail, unordered_map<char, string>& bitTrails) {
    if (node->left == nullptr && node->right == nullptr) {
        bitTrails[node->symbol] = trail;
        return;
    }
    generateBitTrails(node->left.get(), trail + '0', bitTrails);
    generateBitTrails(node->right.get(), trail + '1', bitTrails);
}

// Function to perform encryption (bit-trail generation)
void encrypt(const unordered_map<char, string>& bitTrails, const string& inputFile, const string& outputFile) {
    ifstream input(inputFile);
    ofstream output(outputFile, ios::binary);

    string bitBuffer;  // Buffer to accumulate bits before writing to the output file

    char ch;
    while (input.get(ch)) {
        if (!isprint(ch)) {
            ch = '#';
        }
        const string& bitTrail = bitTrails.at(ch);

        // Debugging output
        cout << "Character: " << ch << ", Bit Trail: " << bitTrail << endl;

        // Append the bit trail to the buffer
        bitBuffer += bitTrail;

        // Write complete bytes to the output file
        while (bitBuffer.length() >= 8) {
            bitset<8> bits(bitBuffer.substr(0, 8));
            output.write(reinterpret_cast<const char*>(&bits), sizeof(bits));
            bitBuffer.erase(0, 8);
        }
    }

    // Write any remaining bits to the output file
    if (!bitBuffer.empty()) {
        bitset<8> bits(bitBuffer);
        output.write(reinterpret_cast<const char*>(&bits), sizeof(bits));
    }

    input.close();
    output.close();
}

// Function to perform decryption (bit-trail traversal)
void decrypt(const Node* root, const string& inputFile, const string& outputFile) {
    ifstream input(inputFile, ios::binary);
    ofstream output(outputFile);

    bitset<8> bits;
    Node* current = const_cast<Node*>(root);

    while (input.read(reinterpret_cast<char*>(&bits), sizeof(bits))) {
        cout << "Read bits: " << bits << endl;

        for (int i = 7; i >= 0; --i) {
            cout << "Processing bit: " << bits[i] << endl;
            if (bits[i]) {
                current = current->right.get();
                cout << "Move right" << endl;
            } else {
                current = current->left.get();
                cout << "Move left" << endl;
            }

            if (!current->left && !current->right) {
                // Debugging output
                cout << "Decoded Character: " << current->symbol << " (ASCII: " << int(current->symbol) << ")" << endl;

                output << current->symbol;
                current = const_cast<Node*>(root); // Reset to the root for the next character
            }
        }
    }

    input.close();
    output.close();
}

int main() {
    // Part 1: Encryption

    // A. Read the file and handle exceptions
    try {
        ifstream inputFile("plenty.txt");
        if (!inputFile.is_open()) {
            cerr << "Error opening the input file." << endl;
            return 1;
        }

        string rawdata;
        char ch;
        while (inputFile.get(ch)) {
            if (!isprint(ch)) {  // Check if the character is printable
                ch = '#';  // Assign a specific character if not printable
            }
            rawdata += ch;
        }
        inputFile.close();

        // B. Frequency counter algorithm
        unordered_map<char, int> frequency;
        for (char ch : rawdata) {
            frequency[ch]++;
        }

        // C. Sort lower ASCII frequency characters
        unique_ptr<Node> root = constructQTree(frequency);

        // D. Create bit-trails for each character
        unordered_map<char, string> bitTrails;
        generateBitTrails(root.get(), "", bitTrails);

        // E. Encrypt the file
        encrypt(bitTrails, "plenty.txt", "encrypted.bin");

        // Part 2: Decryption

        // A. Decrypt the file
        decrypt(root.get(), "encrypted.bin", "decrypted.txt");

        // Clean up allocated memory
        deleteTree(root.get());

    } catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}