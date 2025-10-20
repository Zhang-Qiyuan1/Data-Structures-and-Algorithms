#include <iostream>

using namespace std;

//定义链表节点
struct LinkNode {
    int value;
    LinkNode *next;
};
//--------------------------------------------------

//函数名：InitLink
//功能：初始化链表
//输入：整型数组list，数组长度length
//返回：链表头节点指针
LinkNode *InitLink(int list[], int length) {
    //初始化头节点
    LinkNode *head = new LinkNode;
    head -> value = list[0];
    head -> next = NULL;
    //定义一个临时节点
    LinkNode *tmp = new LinkNode;
    tmp -> next = NULL;
    tmp = head;
    //依次添加后续节点
    for (int i = 1; i < length; i ++ ){
        LinkNode *ptr = new LinkNode;
        ptr -> value = list[i];
        ptr -> next = NULL;
        tmp -> next = ptr;
        tmp = ptr;
    }
    //返回头节点
    return head;
}
//--------------------------------------------------

//函数名：BubbleSort
//功能：对链表进行冒泡排序
//输入：链表头节点指针head，链表长度length
//返回：排序后的链表头节点指针
LinkNode *BubbleSort(LinkNode *head, int length){
    //边界条件判断
    if (head == NULL || head -> next == NULL) return head;

    for (int i = 0; i < length; i++){
        LinkNode *cur = head;
        LinkNode *next = head -> next;
        for (int j = 0; j < length - i - 1; j++){
            //如果满足条件，交换节点值
            if (cur -> value > next -> value){
                int tmp = cur -> value;
                cur -> value = next -> value;
                next -> value = tmp;
            }
            cur = cur -> next;
            next = next -> next;
        }
    }

    return head;

}
//--------------------------------------------------

//函数名：PrintLink
//功能：打印链表
//输入：链表头节点指针head
//返回：无
void PrintLink(LinkNode *head) {
    LinkNode *ptr = head;
    while (ptr != NULL){
        cout << ptr -> value << " ";
        ptr = ptr -> next;
    }
}
//--------------------------------------------------

int main(){
    int list[] = {3, 86, 32, 7, 15, 34, 91, 20, 55, 14};
    int length = sizeof(list) / sizeof(list[0]);

    LinkNode *head = InitLink(list,length);
    BubbleSort(head, length);
    PrintLink(head);
    
    return 0;
}