#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

//结构体名： my_vector
//成员变量： int n; int *a
//功能实现：实现动态数组的基本操作
struct my_vector{
    int n;
    int *a = new int[n];
};
//----------------------------------------------------------

//函数名： PrintSize
//参数： const my_vector &v
//功能实现：输出动态数组的大小
//返回值： 无
void PrintSize(const my_vector &v){
    cout << v.n << endl;
}
//----------------------------------------------------------

//函数名： ShowElements
//参数： const my_vector &v
//功能实现：输出动态数组的所有元素
//返回值： 无
void ShowElements(const my_vector &v){
    for (int i = 0; i < v.n; i++){
        cout << v.a[i] << " ";
    }
    cout << endl;
}
//----------------------------------------------------------

//函数名： PushBack
//参数： my_vector &v, int x
//功能实现：在动态数组末尾添加元素x
//返回值： 无
void PushBack(my_vector &v, int x){
    int *newa = new int[v.n + 1];
    for (int i = 0; i < v.n; i++){
        newa[i] = v.a[i];
    }
    newa[v.n] = x;
    delete[] v.a;
    v.a = newa;
    v.n += 1;
    for (int i = 0; i < v.n; i++){
        cout << v.a[i] << " ";
    }
    cout << endl;
}
//----------------------------------------------------------

//函数名： PopBack
//参数： my_vector &v
//功能实现：删除动态数组末尾的元素
//返回值： 无
void PopBack(my_vector &v){
    if (v.n == 0) return;
    int *newa = new int[v.n - 1];
    for (int i = 0; i < v.n - 1; i++){
        newa[i] = v.a[i];
    }
    delete[] v.a;
    v.a = newa;
    v.n -= 1;
    for (int i = 0; i < v.n; i++){
        cout << v.a[i] << " ";
    }
    cout << endl;
}
//----------------------------------------------------------

//函数名： Insert
//参数： my_vector &v, int p, int x
//功能实现：在动态数组的第p个位置插入元素x
//返回值： 无
void Insert(my_vector &v, int p, int x){
    if (p < 0 || p > v.n) return;
    int *newa = new int[v.n + 1];
    for (int i = 0; i < p - 1; i++){
        newa[i] = v.a[i];
    }
    newa[p - 1] = x;
    for (int i = p - 1; i < v.n; i++){
        newa[i + 1] = v.a[i];
    }
    delete[] v.a;
    v.a = newa;
    v.n += 1;
    for (int i = 0; i < v.n; i++){
        cout << v.a[i] << " ";
    }
    cout << endl;
}
//----------------------------------------------------------

//函数名： Erase
//参数： my_vector &v, int p
//功能实现：删除动态数组的第p个位置的元素
//返回值： 无
void Erase(my_vector &v, int p){
    if (p < 0 || p >= v.n) return;
    int *newa = new int[v.n - 1];
    for (int i = 0; i < p - 1; i++){
        newa[i] = v.a[i];
    }
    for (int i = p; i < v.n; i++){
        newa[i - 1] = v.a[i];
    }
    delete[] v.a;
    v.a = newa;
    v.n -= 1;
    for (int i = 0; i < v.n; i++){
        cout << v.a[i] << " ";
    }
    cout << endl;
}
//----------------------------------------------------------

//函数名： Update
//参数： my_vector &v, int p, int x
//功能实现：更新动态数组第p个位置的元素为x
//返回值： 无
void Update(my_vector &v, int p, int x){
    if (p <= 0 || p >= v.n) return;
    v.a[p - 1] = x;
    for (int i = 0; i < v.n; i++){
        cout << v.a[i] << " ";
    }
    cout << endl;
}
//----------------------------------------------------------

//函数名： Save
//参数： const my_vector &v, const char *file_name
//功能实现：将动态数组保存到指定文件（支持.txt和.bin格式）
//返回值： 无
void Save(const my_vector &v, const char *file_name) {
    // 检查文件扩展名
    const char *dot = strrchr(file_name, '.');
    if (dot == nullptr) {
        cout << "Error: No file extension found." << endl;
        return;
    }
    
    if (strcmp(dot, ".txt") == 0) {
        //cout << "Save as txt file: " << file_name << endl;
        
        // 使用 ofstream 写入文本文件
        ofstream file(file_name);
        if (!file.is_open()) {
            cout << "Error: Cannot open file for writing!" << endl;
            return;
        }
        
        for (int i = 0; i < v.n; i++) {
            file << v.a[i] << " ";
        }
        
        file.close();
        cout << "Saved successfully to D:\\vscode_cpp\\Algorithm\\"<< file_name << "(text)!" << endl;
        
    } else if (strcmp(dot, ".bin") == 0) {
        cout << "Save as bin file: " << file_name << endl;

        ofstream file(file_name, ios::binary);
        if (!file.is_open()) {
            cout << "Error: Cannot open file for writing!" << endl;
            return;
        }
        
        file.write(reinterpret_cast<const char*>(v.a), v.n * sizeof(int));
        
        if (!file) {
            cout << "Error: Failed to write binary data!" << endl;
        } else {
            cout << "Saved successfully to D:\\vscode_cpp\\Algorithm\\ "<< file_name << "(binary)!" << endl;
        }
        
        file.close();
        
    } else {
        cout << "Unsupported file format: " << dot << endl;
        return;
    }
}
//----------------------------------------------------------

//函数名： ReadTxtfile
//参数： 无
//功能实现：从指定文本文件读取动态数组数据
//返回值： my_vector
my_vector ReadTxtfile(){
    my_vector v;
    ifstream file("D:\\vscode_cpp\\Algorithm\\Lab01\\2024datastruct\\input\\exp1_example.txt");  // 使用文件流，不影响stdin
    
    if (!file.is_open()) {
        cout << "Failed to open file!" << endl;
        return v;
    }
    
    file >> v.n;
    for (int i = 0; i < v.n; i++) {
        file >> v.a[i];
    }
    file.close();
    
    cout << "Loaded successfully from Lab01\\2024datastruct\\input\\exp1_example.txt (text)!" << endl;
    return v;
}
//----------------------------------------------------------

//函数名： ReadBinfile
//参数： 无
//功能实现：从指定二进制文件读取动态数组数据
//返回值： my_vector
my_vector ReadBinfile() {
    my_vector v;
    ifstream file("D:\\vscode_cpp\\Algorithm\\Lab01\\2024datastruct\\input\\exp1_example.bin", ios::binary);
    
    if (!file.is_open()) {
        cout << "Error: Cannot open binary file!" << endl;
        return v;
    }

    // 首先读取元素个数
    file.read(reinterpret_cast<char*>(&v.n), sizeof(int));
    
    if (!file) {
        cout << "Error: Failed to read element count from binary file!" << endl;
        file.close();
        return v;
    }

    // 然后读取数组数据
    file.read(reinterpret_cast<char*>(v.a), v.n * sizeof(int));
    
    if (!file) {
        cout << "Error: Failed to read array data from binary file!" << endl;
        file.close();
        return v;
    }
    
    file.close();
    cout << "Loaded successfully from exp1_example.bin (binary)!" << endl;
    return v;
}
//----------------------------------------------------------

//函数名： InputOperation
//参数： 无
//功能实现：根据用户输入执行相应的动态数组操作
//返回值： 无
void InputOperation(){
    char operation[20] = {0};
    my_vector v = ReadBinfile();
    while (1){
        cout << "Please enter the operation you want to perform (show, size, push_back, pop_back, insert, erase, update, save): ";
        cin >> operation;
        if (strcmp(operation, "show") == 0){
            ShowElements(v);
        } else if (strcmp(operation, "size") == 0){
            PrintSize(v);
        } else if (strcmp(operation, "push_back") == 0){
            int x;
            cout << "Enter the value to push back: ";
            cin >> x;
            PushBack(v, x);
        } else if (strcmp(operation, "pop_back") == 0){
            PopBack(v);
        } else if (strcmp(operation, "insert") == 0){
            int p, x;
            cout << "Enter the position and value to insert: ";
            cin >> p >> x;
            Insert(v, p, x);
        } else if (strcmp(operation, "erase") == 0){
            int p;
            cout << "Enter the position to erase: ";
            cin >> p;
            Erase(v, p);
        } else if (strcmp(operation, "update") == 0){
            int p, x;
            cout << "Enter the position and new value to update: ";
            cin >> p >> x;
            Update(v, p, x);
        } else if (strcmp(operation, "save") == 0){
            char file_name[50];
            cout << "Enter the file name to save (with .txt or .bin extension): ";
            cin >> file_name;
            Save(v, file_name);
        }  else if (strcmp(operation, "exit") == 0){
            break;
        } else {
            cout << "Unsupported operation." << endl;
        }
    }
    
}
//----------------------------------------------------------

int main(){
    InputOperation();
    return 0;
}