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
void encode_file(unordered_map<char, string> huffman_code)
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
            cout<<static_cast<int>(buffer)<<" ";

                outFile.put(buffer);
                buffer = 0;
                bitcount = 0;
            }
        }
    }
    if (bitcount > 0)
    {

        buffer = (buffer << (8 - bitcount));
        cout<<static_cast<int>(buffer)<<" ";
        bitcount = 0;
        outFile.put(buffer);
    }
    inFile.close();
    outFile.close();
}
void decode_file(Node *root, const string &encoded_file)
{
    ifstream Infile("test.bin",ios::binary);
    ofstream decoded_file("decoded.txt",ios::binary);
    if(!Infile)
    {
        cout<<"Error in binary bin file during  decoding_file";
    }
    char bytes;
    Node *temp=root;
    while(Infile.get(bytes))
    {
        if(Infile.peek()==EOF)
        {
            
        }
        for(int i=7;i>=0;i--)
        {
            if(!temp->left&&!temp->right)
            {
                decoded_file.put(temp->ch);
                temp=root;
            }
           temp=(bytes&(1<<i))?temp->right:temp->left;
        }
    }
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
    for(auto i:huffman_codes)
    {
        cout<<i.first<<" "<<i.second<<endl;

    }
    encode_file(huffman_codes);
    decode_file(pq.top(),"test.bin");
    return 0;
}
