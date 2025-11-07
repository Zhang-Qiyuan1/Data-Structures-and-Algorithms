#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>
#include "rsa.h"

#define ll long long  // 定义ll为long long类型，方便使用

using namespace std;

// 定义Demo文件路径
string DemoBlockPath = "D:\\vscode_cpp\\Algorithm\\MA02\\demo\\blocks.csv";
string DemoTransactionPath = "D:\\vscode_cpp\\Algorithm\\MA02\\demo\\transactions.csv";
string DemoInputsPath = "D:\\vscode_cpp\\Algorithm\\MA02\\demo\\inputs.csv";
string DemoOutputsPath = "D:\\vscode_cpp\\Algorithm\\MA02\\demo\\outputs.csv";

// 定义Data文件路径
// string DataBlockPath = "D:\\vscode_cpp\\Algorithm\\MA02\\MA1_BlockChain\\2009data\\full\\blocks.csv";
// string DataTransactionPath = "D:\\vscode_cpp\\Algorithm\\MA02\\MA1_BlockChain\\2009data\\full\\transactions.csv";
// string DataInputsPath = "D:\\vscode_cpp\\Algorithm\\MA02\\MA1_BlockChain\\2009data\\full\\inputs.csv";
// string DataOutputsPath = "D:\\vscode_cpp\\Algorithm\\MA02\\MA1_BlockChain\\2009data\\full\\outputs.csv";

// 定义输入结构体
struct Input{
    ll pre_block;          // 该input所引用的output所在区块的高度
    string prevTxID;       // 该input所引用的output所在交易的txID
    ll prevTxOutIndex;     // 该input所引用的output位于所在交易output集合中的索引
    string scriptSig = ""; // 脚本和签名，本实验置空
};

// 定义输出结构体
struct Output{
    string txid;           // 该output所属的交易
    ll index;              // 该output在所属交易中的索引值
    ll value;              // 该output的价值(数据已乘10^8,避免浮点误差)
    string script = "";    // 脚本，本实验置空
};

// 定义交易结构体
struct Transaction{
    string txid;            // 交易编号，具有唯一性
    ll input_count;         // inputs的数量(忽略)
    ll output_count;        // outputs的数量(忽略)
    vector <Input> inputs;  // 一组input的集合，表示当前交易的输入所用到的输出
    vector <Output> outputs;// 一组output的集合，表示当前交易的输出，可能作为后续交易的输入
    int is_coinbase;        // 表示是否为coinbase交易（1为coinbase交易，0为非coinbase交易）
};

// 定义区块结构体
struct Block{
    ll height;                           // 当前块的高度，一条链上每个区块的Height均不相同
    string hash;                         // 本区块的哈希值
    string prevhash = "";                // 前一个区块的哈希值，置空
    string merkleRoot = "";              // 本区块中所有交易的默克尔树根，置空
    ll nonce = 0;                        // 忽略
    vector <Transaction> transactions;   // 一组交易的集合
};

vector <Block> blocks;  // 全局区块向量，存储所有区块数据

// 将字符串转换为长整型数字
ll ChangeToNumber(const string str){
    ll num = 0;
    for (const auto &s : str) {
        num *= 10;
        num += s - '0';
    }
    return num;
}

// 计算中缀表达式
ll Calculate(const string str){
    ll result = 0;
    stack <char> ops; // 运算符栈
    stack <ll> vals;  // 操作数栈
    for (int i = 0; i < str.length(); i++){
        // 跳过空格
        if (str[i] == ' ') continue;
        // 处理数字
        if (str[i] >= '0' && str[i] <= '9'){
            string num = "";
            while (str[i] >= '0' && str[i] <= '9'){
                num += str[i];
                i++;    
            }
            ll number = ChangeToNumber(num);
            vals.push(number);
        }else{
            //左括号直接入栈
            if (str[i] == '('){
                ops.push(str[i]);
            }else if (str[i] == ')'){
                char ch = ops.top();
                while (!ops.empty() && ch != '('){
                    ops.pop();
                    ll num2 = vals.top();
                    vals.pop();
                    ll num1 = vals.top();
                    vals.pop();
                    switch (ch){
                        case '+':
                            vals.push(num1 + num2);
                            break;
                        case '-':
                            vals.push(num1 - num2);
                            break;
                        case '*':
                            vals.push(num1 * num2);
                            break;
                        case '/':
                            vals.push(num1 / num2);
                            break;
                        default:
                            break;
                    }
                    ch = ops.top();
                }
                ops.pop();
            }else {
                switch (str[i]){
                    case '+':
                        while (!ops.empty() && ops.top() != '('){
                            char ch = ops.top();
                            ops.pop();
                            ll num2 = vals.top();
                            vals.pop();
                            ll num1 = vals.top();
                            vals.pop();
                            switch (ch){
                                case '+':
                                    vals.push(num1 + num2);
                                    break;
                                case '-':
                                    vals.push(num1 - num2);
                                    break;
                                case '*':
                                    vals.push(num1 * num2);
                                    break;
                                case '/':
                                    vals.push(num1 / num2);
                                    break;
                                default:
                                    break;
                            }
                        }
                        ops.push('+');
                        break;
                    case '-':
                        while (!ops.empty() && ops.top() != '('){
                            char ch = ops.top();
                            ops.pop();
                            ll num2 = vals.top();
                            vals.pop();
                            ll num1 = vals.top();
                            vals.pop();
                            switch (ch){
                            case '+':
                                vals.push(num1 + num2);
                                break;
                            case '-':
                                vals.push(num1 - num2);
                                break;
                            case '*':
                                vals.push(num1 * num2);
                                break;
                            case '/':
                                vals.push(num1 / num2);
                                break;
                            default:
                                break;
                            }
                        }
                        ops.push('-');
                        break;
                    case '*':
                        while (!ops.empty() && ops.top() != '(' && (ops.top() == '*' || ops.top() == '/')){
                            char ch = ops.top();
                            ops.pop();
                            ll num2 = vals.top();
                            vals.pop();
                            ll num1 = vals.top();
                            vals.pop();
                            switch (ch){
                            case '+':
                                vals.push(num1 + num2);
                                break;
                            case '-':
                                vals.push(num1 - num2);
                                break;
                            case '*':
                                vals.push(num1 * num2);
                                break;
                            case '/':
                                vals.push(num1 / num2);
                                break;
                            default:
                                break;
                            }
                        }
                        ops.push('*');
                        break;
                    case '/':
                        while (!ops.empty() && ops.top() != '(' && (ops.top() == '*' || ops.top() == '/')){
                            char ch = ops.top();
                            ops.pop();
                            ll num2 = vals.top();
                            vals.pop();
                            ll num1 = vals.top();
                            vals.pop();
                            switch (ch){
                            case '+':
                                vals.push(num1 + num2);
                                break;
                            case '-':
                                vals.push(num1 - num2);
                                break;
                            case '*':
                                vals.push(num1 * num2);
                                break;
                            case '/':
                                vals.push(num1 / num2);
                                break;
                            default:
                                break;
                            }
                        }
                        ops.push('/');
                        break;
                    default:
                        break;
                }
            }
        }
    }
    //若栈非空，处理剩下的内容
    while (!ops.empty()){
        char ch = ops.top();
        ops.pop();
        ll num2 = vals.top();
        vals.pop();
        ll num1 = vals.top();
        vals.pop();
        switch (ch){
        case '+':
            vals.push(num1 + num2);
            break;
        case '-':
            vals.push(num1 - num2);
            break;
        case '*':
            vals.push(num1 * num2);
            break;
        case '/':
            vals.push(num1 / num2);
            break;
        default:
            break;
        }
    }
    
    return vals.top();
}

// 读取数据文件并解析到blocks向量中
void ReadDemoFiles(vector <Block> &blocks){
    // 读取区块文件
    ifstream file1(DemoBlockPath);
    string line1;
    getline(file1, line1);  // 跳过标题行
    while (getline(file1, line1)){
        Block blk;  // 创建新区块对象
        
        // 使用stringstream解析CSV行
        stringstream ss(line1);
        string cell;
        int col = 0;
        while (getline(ss, cell, ',')){
            switch (col) {
                case 0:
                    blk.height = ChangeToNumber(cell);  // 解析区块高度
                    break;
                case 1:
                    blk.hash = cell;  // 解析区块哈希
                    break;
                case 2:
                    blk.prevhash = cell;  // 解析前一个区块哈希
                    break;
                case 3:
                    blk.merkleRoot = cell;  // 解析默克尔树根
                    break;
                case 4:
                    blk.nonce = ChangeToNumber(cell);  // 解析nonce值
                    break;
                default:
                    break;
            }
            col ++;
        }
        blocks.push_back(blk);  // 将区块添加到向量中
    }

    // 读取交易文件
    ifstream file2(DemoTransactionPath);
    string line2;
    getline(file2, line2);  // 跳过标题行
    while (getline(file2, line2)){
        Transaction tra;  // 创建新交易对象
        ll hgt;  // 存储区块高度
        
        stringstream ss(line2);
        string cell;
        int col = 0;
        while (getline(ss, cell, ',')){
            switch (col) {
                case 0:
                    hgt = ChangeToNumber(cell);  // 解析区块高度
                    break;
                case 1:
                    tra.txid = cell;  // 解析交易ID
                    break;
                case 2:
                    tra.is_coinbase = ChangeToNumber(cell);  // 解析是否为coinbase交易
                    break;
                case 3:
                    tra.input_count = ChangeToNumber(cell);  // 解析输入数量
                    break;
                case 4:
                    tra.output_count = ChangeToNumber(cell);  // 解析输出数量
                    break;
                default:
                    break;
            }
            col ++;
        }
        // 将交易添加到对应高度的区块中
        for (auto &b : blocks) {
            if (b.height == hgt) {
                b.transactions.push_back(tra);
                break;
            }
        }
    }

    // 读取输入文件
    ifstream file3(DemoInputsPath);
    string line3;
    getline(file3, line3);  // 跳过标题行
    while (getline(file3, line3)) {
        Input ipt;  // 创建新输入对象
        ll hgt;     // 存储区块高度
        string id;  // 存储交易ID
        
        stringstream ss(line3);
        string cell;
        ll col = 0;
        while (getline(ss, cell, ',')){
            switch (col){
                case 0:
                    hgt = ChangeToNumber(cell);  // 解析区块高度
                    break;
                case 1:
                    id = cell;  // 解析交易ID
                    break;
                case 2:
                    ipt.pre_block = ChangeToNumber(cell);  // 解析引用的区块高度
                    break;
                case 3:
                    ipt.prevTxID = cell;  // 解析引用的交易ID
                    break;
                case 4:
                    ipt.prevTxOutIndex = ChangeToNumber(cell);  // 解析引用的输出索引
                    break;
                case 5:
                    ipt.scriptSig = cell;  // 解析脚本签名
                    break;
                default:
                    break;
            }
            col ++;
        }
        // 将输入添加到对应区块和交易中
        for (auto &b : blocks) {
            if(b.height == hgt) {
                for (auto &t : b.transactions) {
                    if (t.txid == id) {
                        t.inputs.push_back(ipt);
                        break;
                    }
                }
            }
        }
    }

    // 读取输出文件
    ifstream file4(DemoOutputsPath);
    string line4;
    getline(file4, line4);  // 跳过标题行
    while (getline(file4, line4)) {
        Output opt;  // 创建新输出对象
        ll hgt;      // 存储区块高度
        
        stringstream ss(line4);
        string cell;
        ll col = 0;
        while (getline(ss, cell, ',')){
            switch (col){
                case 0:
                    hgt = ChangeToNumber(cell);  // 解析区块高度
                    break;
                case 1:
                    opt.txid = cell;  // 解析交易ID
                    break;
                case 2:
                    opt.index = ChangeToNumber(cell);  // 解析输出索引
                    break;
                case 3:
                    opt.value = ChangeToNumber(cell);  // 解析输出价值
                    break;
                case 4:
                    opt.script = cell;  // 解析脚本
                    break;
                default:
                    break;
            }
            col ++;
        }
        // 将输出添加到对应区块和交易中
        for (auto &b : blocks) {
            if(b.height == hgt) {
                for (auto &t : b.transactions) {
                    if (t.txid == opt.txid) {
                        t.outputs.push_back(opt);
                        break;
                    }
                }
            }
        }
    }
}

// 检查签名是否有效
bool CheckSig(const string &pub, const string &sig){
    Rsa rsa;
    BigInt signature(sig);  
    int n = 128;
    rsa.init(n / 2);
    unsigned int public_key = std::stoi(pub);
    rsa.setPu(public_key);
    //进行公钥验证
    bool result = verify(rsa, signature, pub);
    return result;
}

// 判断脚本是否有效
bool JudgeScript(const string &ipt_script, const string &opt_script){
    //拼接字符串
    string added_string = ipt_script + " " + opt_script;
    stringstream ss (added_string);
    string token;
    stack <string> ScriptStack;
    string Calstr;
    //判断是否计算中
    bool isCalcing = false;

    while (ss >> token){
        if (isCalcing){
            if (token == "OP_END_CALC") {
                // 计算表达式结果
                ll result = Calculate(Calstr);
                ScriptStack.push(to_string(result));
                isCalcing = false;
                Calstr.clear();
            } else {
                if (!Calstr.empty()) Calstr += " ";
                Calstr += token;
            }
            continue;
        }
        //按照每个操作符的含义进行操作
        if (token == "OP_HASH160"){
            if (ScriptStack.empty()) return false;
            string tp = ScriptStack.top();
            ScriptStack.pop();
            std::hash<std::string> h;
            size_t hash = h(tp);
            ScriptStack.push(to_string(hash));
        }else if (token == "OP_EQUALVERIFY"){
            if (ScriptStack.size() < 2) return false;
            string tp1 = ScriptStack.top();
            ScriptStack.pop();
            string tp2 = ScriptStack.top();
            ScriptStack.pop();
            return tp1 == tp2;
        }else if (token == "OP_DUP"){
            if (ScriptStack.empty()) return false;
            string tp = ScriptStack.top();
            ScriptStack.push(tp);
        }else if (token == "OP_CHECKSIG"){
            if (ScriptStack.size() < 2) return false;
            string tp1 = ScriptStack.top();
            ScriptStack.pop();
            string tp2 = ScriptStack.top();
            ScriptStack.pop();
            return CheckSig(tp1, tp2);
        }else if (token == "OP_BEGIN_CALC"){
            isCalcing = true;
            continue;
        }else {
            ScriptStack.push(token);
        }
    }
    return ScriptStack.empty() || ScriptStack.top() != "0";
}

// 检查交易是否合法
bool IsLegal(const Transaction &tra, const vector<Block> &blocks, const ll curr_height) {
    // 检查coinbase交易：coinbase交易应该没有输入
    if (tra.is_coinbase == 1) {
        return tra.inputs.empty();  
    }
    
    // 检查每个输入引用的输出是否存在
    for (const auto &input : tra.inputs) {
        bool output_exists = false;
        for (const auto &block : blocks) {
            if (block.height == input.pre_block) {
                for (const auto &transaction : block.transactions) {
                    if (transaction.txid == input.prevTxID) {
                        // 检查引用的输出索引是否有效
                        if (input.prevTxOutIndex < transaction.outputs.size()) {
                            output_exists = true;
                        }
                        break;
                    }
                }
                break; 
            }
        }
        if (!output_exists) {
            return false;  // 如果引用的输出不存在，交易不合法
        }
    }

    // 检查脚本是否有效
    for (const auto &input : tra.inputs){
        bool isMatch = true;
        for (const auto &block : blocks){
            if (block.height == input.pre_block){
                for (const auto &transaction : block.transactions){
                    if (transaction.txid == input.prevTxID){
                        if (input.prevTxOutIndex < transaction.outputs.size()){
                            Output otpt = transaction.outputs[input.prevTxOutIndex];
                            string script = otpt.script;
                            if (script == ""){
                                isMatch = true;
                                break;
                            }
                            // 验证脚本
                            isMatch = JudgeScript(input.scriptSig, script);
                            if (!isMatch) return false;
                        }                                                  
                    }
                }
            }   
        }
    }
    
    // 检查双花问题：同一个输出不能被多次引用
    for (const auto &input : tra.inputs) {
        for (const auto &block : blocks) {
            if (block.height > curr_height) break;
            for (const auto &transaction : block.transactions) {
                if (transaction.txid == tra.txid) continue;  // 跳过当前交易自身
                for (const auto &other_input : transaction.inputs) {
                    // 检查是否有其他交易引用了相同的输出
                    if (other_input.pre_block == input.pre_block &&
                        other_input.prevTxID == input.prevTxID &&
                        other_input.prevTxOutIndex == input.prevTxOutIndex) {
                        cout << input.pre_block << " " << input.prevTxID << " " << input.prevTxOutIndex << endl;
                        return false;  // 存在双花，交易不合法
                    }
                }
            }
        }
    }
    
    
    
    // 检查交易价值平衡：输入总值应大于等于输出总值
    ll total_input_value = 0;
    ll total_output_value = 0;
    
    // 计算所有输入的总价值
    for (const auto &input : tra.inputs) {
        for (const auto &block : blocks) {
            if (block.height == input.pre_block) {
                for (const auto &transaction : block.transactions) {
                    if (transaction.txid == input.prevTxID) {
                        if (input.prevTxOutIndex < transaction.outputs.size()) {
                            total_input_value += transaction.outputs[input.prevTxOutIndex].value;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    
    // 计算所有输出的总价值
    for (const auto &output : tra.outputs) {
        total_output_value += output.value;
    }

    // 检查输入是否足够支付输出
    return total_input_value >= total_output_value;
}

// 打印统计信息
void PrintStatisticInfo(const vector <Block> &blocks){
    ll block_count = 0, trans_count = 0, legal_trans = 0;
    
    // 遍历所有区块和交易进行统计
    for (const auto &b : blocks) {
        block_count ++;
        for (const auto &tra : b.transactions){
            trans_count ++;
            if (IsLegal(tra,blocks,b.height)) legal_trans++;  // 统计合法交易数量
        }
    }
    
    // 输出统计结果
    cout << "Length of Block chains: " << block_count << endl
         << "Total number of Block chains: " << block_count << endl
         << "Total number of legal transactions: " << legal_trans << endl
         << "Total number of illegal transactions: " << trans_count - legal_trans << endl;
    cout << endl;
}

// 打印指定高度的区块信息
void PrintBlockInfo(const ll hgt, const vector <Block> blocks) {
    bool found = false;
    // 查找指定高度的区块
    for (const auto &b : blocks){
        if (b.height == hgt){
            found = true;
            // 输出区块详细信息
            cout << "Block Information: " << endl;
            cout << "Height: " << b.height << endl
                 << "Hash: " << b.hash << endl
                 << "PrevHash: " << b.prevhash << endl
                 << "MerkleRoot: " << b.merkleRoot << endl
                 << "Nonce: " << b.nonce << endl;
            cout << endl;
            break;
        }
    }
    if (!found) {
        cout << "Block not found!" << endl;
    }
}

// 打印指定交易ID的交易信息
void PrintTransactionInfo(const string TXID, const vector <Block> blocks){
    bool found = false;
    // 查找指定交易ID的交易
    for (const auto &b : blocks) {
        for (const auto &tra : b.transactions) {
            if (tra.txid == TXID) {
                found = true;
                // 输出交易详细信息
                cout << "Transaction Information: " << endl;
                cout << "TXID: " << tra.txid << endl
                     << "input_count: " << tra.input_count << endl
                     << "output_count: " << tra.output_count << endl
                     << "is_coinbase: " << tra.is_coinbase << endl;
                cout << endl;
            }
        }
    }
    if (!found) {
        cout << "Transaction not found!" << endl;
    }
}



int main(){
    //读取文件
    ReadDemoFiles(blocks);
    //打印统计信息
    PrintStatisticInfo(blocks);

    //提示用户输入需要查询的内容
    ll height;
    string TXID;
    cout << "Please enter the block height you want to query: ";
    cin >> height;
    PrintBlockInfo(height, blocks);

    cout << "Please enter the transaction ID you want to query: ";
    cin >> TXID;
    PrintTransactionInfo(TXID, blocks);

    return 0;
}