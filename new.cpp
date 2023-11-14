#include <iostream>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <memory>
#include <sstream>
#include "qtree.cpp"  // Include the QTree implementation

using namespace std;

// Function to encrypt using QTree
void encrypt(const unordered_map<char, string>& bitTrails, const string& inputFile, const string& outputFile) {
    ifstream input(inputFile);
    ofstream output(outputFile, ios::binary);

    char ch;
    while (input.get(ch)) {
        const string& bitTrail = bitTrails.at(ch);
        bitset<8> bits(bitTrail);
        output.write(reinterpret_cast<const char*>(&bits), sizeof(bits));
    }

    input.close();
    output.close();
}

// Function to decrypt using QTree
// Function to perform decryption (bit-trail traversal)
void decrypt(const NodePtr& root, const string& inputFile, const string& outputFile) {
    ifstream input(inputFile, ios::binary);
    ofstream output(outputFile);

    bitset<8> bits;
    NodePtr current = root;

    while (input.read(reinterpret_cast<char*>(&bits), sizeof(bits))) {
        for (int i = 7; i >= 0; --i) {
            if (bits.test(i)) {
                current = dynamic_pointer_cast<Branch>(current)->right();
            } else {
                current = dynamic_pointer_cast<Branch>(current)->left();
            }

            if (!dynamic_pointer_cast<Branch>(current)) {
                // Leaf node reached, write the symbol to the output
                output << dynamic_pointer_cast<Leaf>(current)->symbol();
                current = root;  // Reset to the root for the next bit
            }
        }

    }

    input.close();
    output.close();
}


int main() {
    std::string filename = "plenty.txt";  // Replace with your filename

    // Read character frequencies from the file
    std::vector<std::tuple<char, int, string>> charFrequencyVector = readCharacterFrequency(filename);

    // Build the priority queue tree using QTree implementation
    NodePtr root = buildQueueTree(charFrequencyVector);
    string bitString = "";
    bitTrail(root, bitString);

    // Now, you can use the 'root' and 'bitString' for encryption and decryption

    // Part 1: Encryption
    encrypt(bitTrails, filename, "encrypted.bin");

    // Part 2: Decryption
    decrypt(root, "encrypted.bin", "decrypted.txt");

    return 0;
}
