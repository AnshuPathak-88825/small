#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

class Node
{
public:
    char ch;
    double freq;
    Node *left, *right;

    Node(char c, double f)
    {
        ch = c;
        freq = f;
        left = right = nullptr;
    }
};

struct Compare
{
    bool operator()(const Node *a, const Node *b)
    {
        return a->freq > b->freq;
    }
};
void getHuffmanCode(Node *root, string s, unordered_map<char, string> &huffman_codes)
{
    if (!root)
        return;

    if (!root->left && !root->right)
    {
        if (s == "")
        {
            s = "0";
        }
        huffman_codes[root->ch] = s;
    }

    getHuffmanCode(root->left, s + '0', huffman_codes);
    getHuffmanCode(root->right, s + '1', huffman_codes);
}

void deleteTree(Node *root)
{
    if (!root)
        return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}
void encode_huffman_tree(Node *root, ofstream &outFile, int8_t &buffer, int &bitcount)
{
    if (bitcount == 8)
    {
        outFile.put(buffer);
        buffer = 0;
        bitcount = 0;
    }
    if (!root)
    {
        return;
    }
    if (!root->left && !root->left)
    {
        buffer = buffer << 1 | (1);
        bitcount++;
        if (bitcount == 8)
        {
            outFile.put(buffer);
            buffer = 0;
            bitcount = 0;
        }
        char ch = root->ch;
        for (int i = 7; i >= 0; i--)
        {
            buffer = (buffer << 1) | ((ch >> i) & (1));
            bitcount++;
            if (bitcount == 8)
            {

                outFile.put(buffer);
                buffer = 0;
                bitcount = 0;
            }
        }
        return;
    }
    else
    {
        buffer = buffer << 1 | (0);
        bitcount++;
    }

    encode_huffman_tree(root->left, outFile, buffer, bitcount);

    encode_huffman_tree(root->right, outFile, buffer, bitcount);
}
void encode_file()
{
    fs::path current_path = fs::current_path();
    cout << "Current Directory: " << current_path << endl;

    // List files in the current directory
    cout << "Files in this directory:\n";
    for (const auto &entry : fs::directory_iterator(current_path))
    {
        if (fs::is_regular_file(entry))
        {
            cout << "  " << entry.path().filename() << endl;
        }
    }

    // Ask user for file name
    string filename;
    cout << "\nEnter file name to compress: ";
    cin >> filename;
    ifstream inFile(filename);
    if (!inFile)
    {
        cout << "Error in loading file\n";
        return;
    }

    char ch;
    unordered_map<char, int> freq;
    int total_count = 0;
    while (inFile.get(ch))
    {
        freq[ch]++;
        total_count++;
    }

    priority_queue<Node *, vector<Node *>, Compare> pq;
    for (const auto &entry : freq)
    {
        double probability = double(entry.second) / total_count;
        pq.push(new Node(entry.first, probability));
    }

    while (pq.size() > 1)
    {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();

        Node *root = new Node('\0', left->freq + right->freq);
        root->left = left;
        root->right = right;
        pq.push(root);
    }

    unordered_map<char, string> huffman_code;

    getHuffmanCode(pq.top(), "", huffman_code);
    Node *root = pq.top();
    string encoded_file;
    cout<<"Enter Encoded file name ";
    cin>>encoded_file;
    ofstream outFile(encoded_file, ios::binary);
    if (!outFile)
    {
        cout << "error while opening the file";
        return;
    }
    inFile.clear();
    inFile.seekg(0, ios::beg);
    if (!inFile)
    {
        cout << "Error in loading file\n";
        return;
    }
    int8_t buffer = 0;
    int bitcount = 0;
    string encoded_test = "";

    encode_huffman_tree(root, outFile, buffer, bitcount);
    if (bitcount > 0)
    {

        buffer = (buffer << (8 - bitcount));
        bitcount = 0;
        outFile.put(buffer);
    }
    buffer = 0;
    bitcount = 0;
    outFile.put('@');
    outFile.put('#');
    outFile.put('$');
    int count = 0;
    while (inFile.get(ch))
    {
        string code = huffman_code[ch];
        for (auto bc : code)
        {
            buffer = (buffer << 1) | (bc - '0');

            encoded_test += to_string(bc - '0');

            bitcount++;
            if (bitcount == 8)
            {

                outFile.put(buffer);
                buffer = 0;
                bitcount = 0;
            }
        }
    }
    if (bitcount > 0)
    {

        buffer = (buffer << (8 - bitcount));
        bitcount = 0;
        outFile.put(buffer);
    }
    inFile.close();
    outFile.close();
}
int main()
{
    encode_file();
}