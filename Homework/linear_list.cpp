#include <iostream>
#include <algorithm>
using namespace std;

int main(){
    int list[] = {56, 54, 7, 36, 8, 15, 46, 17};
    int length = sizeof(list) / sizeof(list[0]);
    //定义头尾指针
    int front = 0, back = length - 1;
    while (front < back) {
        while (list[front] % 2 == 1) {
            front ++;
        }
        while (list[back] % 2 == 0) {
            back --;
        }
        if (front < back) {
            swap(list[front],list[back]);
        }
    }
    for (int i = 0; i < length; i ++) cout << list[i] << " ";
    return 0;
    
}
//时间复杂度：O(n)
//空间复杂度：O(1)
