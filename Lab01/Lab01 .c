#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//结构体名： my_vector
//成员变量： int n; int *a
//功能实现：实现动态数组的基本操作
typedef struct {
    int n;
    int *a;
}my_vector;
//----------------------------------------------------------

//函数名： PrintSize
//参数： const my_vector &v
//功能实现：输出动态数组的大小
//返回值： 无
void PrintSize(const my_vector v){
    printf("%d\n", v.n);
}
//----------------------------------------------------------

//函数名： ShowElements
//参数： const my_vector &v
//功能实现：输出动态数组的所有元素
//返回值： 无
void ShowElements(const my_vector v){
    for (int i = 0; i < v.n; i++){
        printf("%d ", v.a[i]);
    }
    printf("\n");
}
//----------------------------------------------------------

//函数名： PushBack
//参数： my_vector &v, int x
//功能实现：在动态数组末尾添加元素x
//返回值： 无
void PushBack(my_vector v, int x){
    int *newa;
    for (int i = 0; i < v.n; i++){
        newa[i] = v.a[i];
    }
    newa[v.n] = x;
    //delete[] v.a;
    v.a = newa;
    v.n += 1;
    for (int i = 0; i < v.n; i++){
        printf("%d ", v.a[i]);
    }
    printf("\n");
}
//----------------------------------------------------------

//函数名： PopBack
//参数： my_vector &v
//功能实现：删除动态数组末尾的元素
//返回值： 无
void PopBack(my_vector v){
    if (v.n == 0) return;
    int *newa;
    for (int i = 0; i < v.n - 1; i++){
        newa[i] = v.a[i];
    }
    //delete[] v.a;
    v.a = newa;
    v.n -= 1;
    for (int i = 0; i < v.n; i++){
        printf("%d ", v.a[i]);
    }
    printf("\n");
}
//----------------------------------------------------------

//函数名： Insert
//参数： my_vector &v, int p, int x
//功能实现：在动态数组的第p个位置插入元素x
//返回值： 无
void Insert(my_vector v, int p, int x){
    if (p < 0 || p > v.n) return;
    int *newa = (int*)malloc((v.n + 1) * sizeof(int));
    for (int i = 0; i < p - 1; i++){
        newa[i] = v.a[i];
    }
    newa[p - 1] = x;
    for (int i = p - 1; i < v.n; i++){
        newa[i + 1] = v.a[i];
    }
    free(v.a);
    v.a = newa;
    v.n += 1;
    for (int i = 0; i < v.n; i++){
        printf("%d ", v.a[i]);
    }
    printf("\n");
}
//----------------------------------------------------------

//函数名： Erase
//参数： my_vector &v, int p
//功能实现：删除动态数组的第p个位置的元素
//返回值： 无
void Erase(my_vector v, int p){
    if (p < 0 || p >= v.n) return;
    int *newa = (int*)malloc((v.n - 1) * sizeof(int));
    for (int i = 0; i < p - 1; i++){
        newa[i] = v.a[i];
    }
    for (int i = p; i < v.n; i++){
        newa[i - 1] = v.a[i];
    }
    free(v.a);
    v.a = newa;
    v.n -= 1;
    for (int i = 0; i < v.n; i++){
        printf("%d ", v.a[i]);
    }
    printf("\n");
}
//----------------------------------------------------------

//函数名： Update
//参数： my_vector &v, int p, int x
//功能实现：更新动态数组第p个位置的元素为x
//返回值： 无
void Update(my_vector v, int p, int x){
    if (p <= 0 || p >= v.n) return;
    v.a[p - 1] = x;
    for (int i = 0; i < v.n; i++){
        printf("%d ", v.a[i]);
    }
    printf("\n");
}
//----------------------------------------------------------

//函数名： Save
//参数： const my_vector &v, const char *file_name
//功能实现：将动态数组保存到指定文件（支持.txt和.bin格式）
//返回值： 无
void Save(const my_vector v, const char *file_name) {
    // 检查文件扩展名
    const char *dot = strrchr(file_name, '.');
    if (dot == NULL) {
        printf("Error: No file extension found.\n");
        return;
    }
    
    if (strcmp(dot, ".txt") == 0) {
        FILE *file = fopen(file_name, "w");
        if (!file) {
            printf("Error: Cannot open file for writing!\n");
            return;
        }
        
        for (int i = 0; i < v.n; i++) {
            fprintf(file, "%d ", v.a[i]);
        }

        fclose(file);
        printf("Saved successfully to D:\\vscode_cpp\\Algorithm\\%s(text)!\n", file_name);
        
    } else if (strcmp(dot, ".bin") == 0) {
        printf("Save as bin file: %s\n", file_name);

        FILE *file = fopen(file_name, "wb");
        if (!file) {
            printf("Error: Cannot open file for writing!\n");
            return;
        }

        fwrite(v.a, sizeof(int), v.n, file);
        
        if (!file) {
            printf("Error: Failed to write binary data!\n");
        } else {
            printf("Saved successfully to D:\\vscode_cpp\\Algorithm\\%s(binary)!\n", file_name);
        }
        
        fclose(file);
        
    } else {
        printf("Unsupported file format: %s\n", dot);
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
    v.n = 0;
    v.a = NULL;
    
    FILE *file = fopen("D:\\vscode_cpp\\Algorithm\\Lab01\\2024datastruct\\input\\exp1_example.txt", "r");
    if (!file) {
        printf("Failed to open file!\n");
        return v;
    }
    fscanf(file, "%d", &v.n);
    v.a = (int*)malloc(v.n * sizeof(int));
    //fscanf(file, "%d", &v.n);
    for (int i = 0; i < v.n; i++) {
        fscanf(file, "%d", &v.a[i]); 
    }
    
    fclose(file);
    printf("Loaded successfully from Lab01\\2024datastruct\\input\\exp1_example.txt (text)!\n");
    return v;
}
//----------------------------------------------------------

//函数名： ReadBinfile
//参数： 无
//功能实现：从指定二进制文件读取动态数组数据
//返回值： my_vector
my_vector ReadBinfile() {
    my_vector v;
    FILE *file = fopen("D:\\vscode_cpp\\Algorithm\\Lab01\\2024datastruct\\input\\exp1_example.bin", "rb");
    if (!file) {
        printf("Error: Cannot open binary file!\n");
        return v;
    }

    // 首先读取元素个数
    fread(&v.n, sizeof(int), 1, file);
    
    if (!file) {
        printf("Error: Failed to read the number of elements from binary file!\n");
        fclose(file);
        return v;
    }

    // 然后读取数组数据
    v.a = (int*)malloc(v.n * sizeof(int));
    fread(v.a, sizeof(int), v.n, file);
    
    if (!file) {
        printf("Error: Failed to read array data from binary file!\n");
        fclose(file);
        return v;
    }

    fclose(file);
    printf("Loaded successfully from Lab01\\2024datastruct\\input\\exp1_example.bin (binary)!\n");
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
        printf("Please enter the operation you want to perform (show, size, push_back, pop_back, insert, erase, update, save): ");
        scanf("%s", operation);
        if (strcmp(operation, "show") == 0){
            ShowElements(v);
        } else if (strcmp(operation, "size") == 0){
            PrintSize(v);
        } else if (strcmp(operation, "push_back") == 0){
            int x;
            printf("Enter the value to push back: ");
            scanf("%d", &x);
            PushBack(v, x);
        } else if (strcmp(operation, "pop_back") == 0){
            PopBack(v);
        } else if (strcmp(operation, "insert") == 0){
            int p, x;
            printf("Enter the position and value to insert: ");
            scanf("%d %d", &p, &x);
            Insert(v, p, x);
        } else if (strcmp(operation, "erase") == 0){
            int p;
            printf("Enter the position to erase: ");
            scanf("%d", &p);
            Erase(v, p);
        } else if (strcmp(operation, "update") == 0){
            int p, x;
            printf("Enter the position and new value to update: ");
            scanf("%d %d", &p, &x);
            Update(v, p, x);
        } else if (strcmp(operation, "save") == 0){
            char file_name[50];
            printf("Enter the file name to save (with .txt or .bin extension): ");
            scanf("%s", file_name);
            Save(v, file_name);
        }  else if (strcmp(operation, "exit") == 0){
            break;
        } else {
            printf("Unsupported operation.\n");
        }
    }
    
}
//----------------------------------------------------------

int main(){
    InputOperation();
    return 0;
}