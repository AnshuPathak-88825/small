#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>

using namespace std;

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
void read_bin(ifstream &inFile)
{
    char ch;
    while (inFile.get(ch))
    {
        for (int i = 7; i >= 0; i--)
        {
            bool bin = (1 << i) & (ch);
        }
    }
}
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
void encode_file(unordered_map<char, string> huffman_code, Node *root)
{
    ofstream outFile("test.bin", ios::binary);
    if (!outFile)
    {
        cout << "error while opening the file";
        return;
    }
    ifstream inFile("big.txt");
    if (!inFile)
    {
        cout << "Error in loading file\n";
        return;
    }
    char ch;
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
    int count=0;
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
bool checkStopSequence(ifstream &inFile) {
    streampos pos = inFile.tellg();
    unsigned char first = inFile.get();
    unsigned char second = inFile.get();
    unsigned char third = inFile.get();
    
    if (inFile.eof()) {
        return false;
    }
    if (first == '@' && second == '#' && third == '$') {
        return true; 
    }
    
    inFile.seekg(pos, ios::beg); 
    return false;
}
bool getNextBit(ifstream &inFile, char &buffer, int &bit_count)
{
    if (bit_count == 8)
    {
        if (checkStopSequence(inFile))
        {
            throw runtime_error("Stop sequence detected. Stopping tree creation.");
        }
        char newbyte = inFile.get();
        if (newbyte == EOF)
        {
            throw "Unexpected end of file while reading bits.";
        }
        buffer = newbyte;
        bit_count = 0;
    }
    bool value = (buffer >> (7 - bit_count)) & 1;
    bit_count++;
    return value;
}
char extractCharacter(ifstream &inFile, int &bit_count, char &buffer)
{
    unsigned char c = 0;
    for (int i = 0; i < 8; i++)
    {
        c = (c << 1) | getNextBit(inFile, buffer, bit_count);
    }
    return c;
}
Node *Create_Tree(ifstream &inFile, int &bit_count, char &buffer)
{
    bool value = getNextBit(inFile, buffer, bit_count);
    if (value == 1)
    {
        unsigned char ch = extractCharacter(inFile, bit_count, buffer);
        Node *leaf = new Node(static_cast<char>(ch), 0);
        return leaf;
    }
    else
    {

        Node *root = new Node('\0', 0);
        root->left = Create_Tree(inFile, bit_count, buffer);
        root->right = Create_Tree(inFile, bit_count, buffer);
        return root;
    }
}
void decode_file(const string &encoded_file)
{
    ifstream Infile("test.bin", ios::binary);
    ofstream decoded_file("decoded.txt", ios::binary);
    if (!Infile)
    {
        cout << "Error in binary bin file during  decoding_file";
    }
    int bit_count = 0;
    char buffer = Infile.get();
    Node *root = Create_Tree(Infile, bit_count, buffer);
    unordered_map<char, string> huffman_codes;
    char bytes;
    Node *temp = root;
    Infile.get();
    Infile.get();
    Infile.get();
    
    while (Infile.get(bytes))
    {
        if (Infile.peek() == EOF)
        {
        }
        for (int i = 7; i >= 0; i--)
        {
            temp = (bytes & (1 << i)) ? temp->right : temp->left;

            if (!temp->left && !temp->right)
            {
                decoded_file.put(temp->ch);
                temp = root;
            }
        }
    }
    Infile.close();
    decoded_file.close();
}

int main()
{
    ifstream inFile("big.txt");
    if (!inFile)
    {
        cout << "Error in loading file\n";
        return 0;
    }

    char ch;
    unordered_map<char, int> freq;
    int total_count = 0;
    while (inFile.get(ch))
    {
        freq[ch]++;
        total_count++;
    }
    inFile.close();

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

    unordered_map<char, string> huffman_codes;

    getHuffmanCode(pq.top(), "", huffman_codes);
    encode_file(huffman_codes, pq.top());
    decode_file("test.bin");
    return 0;
}
