#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>
#include <chrono>
#include <windows.h>
#include <iomanip>
#include <limits> 
#include <cmath>

using namespace std;
using namespace std::chrono;   //用于⏲计时
#define ll long long
#define inf 0x3ffff

//文件路径
const char *Path = "D:\\vscode_cpp\\Algorithm\\MA03\\flight01.csv";

struct Flight{
    ll FlightID;                //航班ID
    ll DepartureDate;           //启航日期
    bool IsIntl;                //航班类型（国际1/国内0）
    string FlightNo;            //航班编号
    ll DepartureAirport;        //启航机场ID
    ll ArrivalAirport;          //抵达机场ID
    double DepartureTime;           //启航时间
    double ArrivalTime;             //抵达时间
    ll AirplaneID;              //飞机ID
    int AirplaneModel;          //飞机机型
    ll AirFares;                //基础费用
    ll PeakSeasonRates;         //旺季价格
    ll OffSeasonRates;          //淡季价格
};

ll PrimeNumberTable[15000] = {0};

// 构造素数表
void SelectPrimeNumber(){
    // 值为0认为是素数
    PrimeNumberTable[0] = 1;
    PrimeNumberTable[1] = 1;
    ll t = 2;
    while (t < 15000){
        ll j = 2;
        if (PrimeNumberTable[t] == 0){
            while (t * j < 15000){
                PrimeNumberTable[t * j] = 1;
                j++;
            }
        }
        t++;
    }
}

// 将字符串转换为长整型数字
ll ChangeToLong(const string str){
    ll num = 0;
    for (const auto &s : str) {
        num *= 10;
        num += s - '0';
    }
    return num;
}

// 将字符串转换为双精度浮点型数字
double ChangeToDouble(const string str){
    double num = 0.0;
    bool decimal_found = false;
    double decimal_place = 0.1;

    for (const auto &s : str) {
        if (s == '.') {
            decimal_found = true;
            continue;
        }
        if (!decimal_found) {
            num *= 10;
            num += s - '0';
        } else {
            num += (s - '0') * decimal_place;
            decimal_place *= 0.1;
        }
    }
    return num;
}

// 数据初始化
vector <Flight> ReadFile(const char *Path){
    cout << "数据初始化中..." << endl;
    auto start = high_resolution_clock::now(); 

    vector<Flight> flights;

    FILE *file = fopen(Path,"r");
    if (file == NULL){
        cout << "文件打开失败!" << endl;
        return flights;
    }
    char line[1024] = {0};
    //跳过标题行
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)){
        line[strcspn(line, "\r\n")] = 0;
        // 用 strtok 按逗号切割
        char *token = strtok(line, ",");
        int cnt = 0;
        Flight flight;
        while (token != NULL) {
            switch (cnt){
                case 0:
                    flight.FlightID = ChangeToLong(string(token));
                    break;
                case 1:
                    flight.DepartureDate = ChangeToLong(string(token));
                    break;
                case 2:
                    if (string(token) == "Dome"){
                        flight.IsIntl = 0;
                    }else flight.IsIntl = 1;
                    break;
                case 3:
                    flight.FlightNo = string(token);
                    break;
                case 4:
                    flight.DepartureAirport = ChangeToLong(string(token));
                    break;
                case 5:
                    flight.ArrivalAirport = ChangeToLong(string(token));
                    break;
                case 6:
                    flight.DepartureTime = ChangeToDouble(string(token));
                    break;
                case 7:
                    flight.ArrivalTime = ChangeToDouble(string(token));
                    break;
                case 8:
                    flight.AirplaneID = ChangeToLong(string(token));
                    break;
                case 9:
                    flight.AirplaneModel = int(ChangeToLong(string(token)));
                    break;
                case 10:
                    flight.AirFares = ChangeToLong(string(token));
                    break;
                case 11:
                    flight.PeakSeasonRates = ChangeToLong(string(token));
                    break;
                case 12:
                    flight.OffSeasonRates = ChangeToLong(string(token));
                    break;
                default:
                    break;
            }
            cnt++;
            token = strtok(NULL, ",");
        }
        flights.push_back(flight);
    }

    fclose(file);

    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据初始化完成！用时：" << duration.count() << " 毫秒" << endl;

    return flights;
}

// 排序函数
bool CompareFlight(const Flight &f1, const Flight &f2){
    if (f1.DepartureAirport == f2.DepartureAirport){
        if (f1.ArrivalAirport == f2.ArrivalAirport){
            return (f1.ArrivalTime - f1.DepartureTime) - (f2.ArrivalTime - f2.DepartureTime) < -1e-7;
        }
        return f1.ArrivalAirport > f2.ArrivalAirport;
    }
    return f1.DepartureAirport > f2.DepartureAirport;
}

// 数据排序
void SortData(vector <Flight> &flights){
    cout << "数据排序中..." << endl;
    auto start = high_resolution_clock::now(); 

    sort(flights.begin(), flights.end(), CompareFlight);
    
    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据排序完成！用时：" << duration.count() << " 毫秒" << endl;
}

//数据查询：基础费用为素数的所有航班
void SearchPrimeFare(vector <Flight> &f){
    cout << "数据查询中..." << endl;
    auto start = high_resolution_clock::now(); 

    vector <Flight> PrimeFareFlights;
    for (const auto flight : f){
        if (!PrimeNumberTable[flight.AirFares]){
            PrimeFareFlights.push_back(flight);
        }
    }

    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据查询完成！用时：" << duration.count() << " 毫秒" << endl;
    if (PrimeFareFlights.empty()){
        cout << "不存在基础费用为素数的航班！" << endl;
        return;
    }
    cout << "基础费用为素数的航班的航班ID：" << endl;
    for (const auto &f : PrimeFareFlights){
        cout << f.FlightID << endl;
    }
}

//二分查找
pair<ll, ll> BinarySearch(vector<Flight> &f, const ll &DepartureID, const ll &ArrivalID){
    if (f.empty()) return {-1, -1};

    // 查找左边界（第一个满足条件的）
    int left = 0, right = f.size() - 1;
    int first_pos = -1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        // 由于是降序，比较逻辑需要调整
        if (f[mid].DepartureAirport > DepartureID) {
            left = mid + 1;
        } else if (f[mid].DepartureAirport < DepartureID) {
            right = mid - 1;
        } else {
            // DepartureAirport 相等，比较 ArrivalAirport
            if (f[mid].ArrivalAirport > ArrivalID) {
                left = mid + 1;
            } else if (f[mid].ArrivalAirport < ArrivalID) {
                right = mid - 1;
            } else {
                // 找到匹配，向左找第一个
                first_pos = mid;
                right = mid - 1;
            }
        }
    }
    
    if (first_pos == -1) return {-1, -1};

    // 查找右边界（最后一个满足条件的）
    left = first_pos;
    right = f.size() - 1;
    int last_pos = first_pos;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (f[mid].DepartureAirport == DepartureID && f[mid].ArrivalAirport == ArrivalID) {
            last_pos = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return {first_pos, last_pos + 1};
}

//数据查询：鞍点航班
void SearchSaddlePointFlight(vector <Flight> &f, const ll &DepartureID, const ll &ArrivalID){
    cout << "数据查询中..." << endl;
    auto start = high_resolution_clock::now(); 

    // 通过二分查找获取指定启航机场和抵达机场的航班范围
    auto result = BinarySearch(f, DepartureID, ArrivalID);
    ll st = result.first;
    ll nd = result.second;

    // 未找到对应航班
    if (st == -1){
        cout << "不存在启航、抵达机场分别为 (" << DepartureID << ", " << ArrivalID << ") 的一组数据" << endl; 
        return;
    }

    // 构造矩阵
    vector <vector<ll>> Matrix;
    for (int i = st; i < nd; i++){
        vector <ll> line;
        line.push_back(f[i].AirFares);
        line.push_back(f[i].PeakSeasonRates);
        line.push_back(f[i].OffSeasonRates);
        line.push_back(f[i].FlightID);
        Matrix.push_back(line);
    }

    // for (auto const &flight : f){
    //     if (flight.DepartureAirport == DepartureID && flight.ArrivalAirport == ArrivalID){
    //         vector <ll> line;
    //         line.push_back(flight.AirFares);
    //         line.push_back(flight.PeakSeasonRates);
    //         line.push_back(flight.OffSeasonRates);
    //         line.push_back(flight.FlightID);
    //         Matrix.push_back(line);
    //     }
    // }

    // 处理非法查询
    if (Matrix.empty()){
        auto end = high_resolution_clock::now(); 
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "数据查询完成！用时：" << duration.count() << " 毫秒" << endl;
        cout << "不存在启航、抵达机场分别为 (" << DepartureID << ", " << ArrivalID << ") 的一组数据" << endl; 
        return;
    }
    // 查找鞍点
    ll MininCol1 = inf, MininCol2 = inf, MininCol3 = inf;
    ll MinID1 = -1, MinID2 = -1, MinID3 = -1;
    for (int row = 0; row < Matrix.size(); row++){
        if (Matrix[row][0] < MininCol1){
            MininCol1 = Matrix[row][0];
            if (Matrix[row][0] > Matrix[row][1] && Matrix[row][0] > Matrix[row][2]){
                MinID1 = Matrix[row][3];
            }else MinID1 = -1;
        }
        if (Matrix[row][1] < MininCol2){
            MininCol2 = Matrix[row][1];
            if (Matrix[row][1] > Matrix[row][0] && Matrix[row][1] > Matrix[row][2]){
                MinID2 = Matrix[row][3];
            }else MinID2 = -1;
        }
        if (Matrix[row][2] < MininCol3){
            MininCol3 = Matrix[row][2];
            if (Matrix[row][2] > Matrix[row][0] && Matrix[row][2] > Matrix[row][1]){
                MinID3 = Matrix[row][3];
            }else MinID3 = -1;
        }
    }

    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据查询完成！用时：" << duration.count() << " 毫秒" << endl;
    if (MinID1 == -1 && MinID2 == -1 && MinID3 == -1){
        cout << "不存在鞍点航班！" << endl;
        return;
    }
    cout << "鞍点航班的航班ID：" << endl;
    if (MinID1 != -1) cout << MinID1 << endl;
    if (MinID2 != -1) cout << MinID2 << endl;
    if (MinID3 != -1) cout << MinID3 << endl;
}

//数据查询：指定飞机ID的执飞顺序
struct AirplaneFlight{
    ll FlightID;
    double DepartureTime;
    double ArrivalTime;
    ll DepartureAirport;
    ll ArrivalAirport;
    AirplaneFlight *next = nullptr;
};

bool CompareDepartureTime(const Flight &f1, const Flight &f2){
    return f1.DepartureTime < f2.DepartureTime;
}

void SearchAirPlaneFlightOrder(vector <Flight> &f, const ll &AirplaneID){
    cout << "数据查询中..." << endl;
    auto start = high_resolution_clock::now(); 
    // 筛选出指定飞机ID的所有航班
    vector <Flight> PointedIDAirplane;
    for (const auto &flight : f){
        if (flight.AirplaneID == AirplaneID){
            PointedIDAirplane.push_back(flight);
        }
    }
    // 处理特殊情况
    if (PointedIDAirplane.empty()){
        auto end = high_resolution_clock::now(); 
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "数据查询完成！用时：" << duration.count() << " 毫秒" << endl;
        cout << "不存在ID为 " << AirplaneID << " 的飞机";
        return;
    }
    sort (PointedIDAirplane.begin(), PointedIDAirplane.end(), CompareDepartureTime);
    AirplaneFlight *head = new AirplaneFlight;
    AirplaneFlight *cur = head;
    for (const auto &flight : PointedIDAirplane){
        AirplaneFlight *linknode = new AirplaneFlight;
        linknode -> FlightID = flight.FlightID;
        linknode -> DepartureTime = flight.DepartureTime;
        linknode -> ArrivalTime = flight.ArrivalTime;
        linknode -> DepartureAirport = flight.DepartureAirport;
        linknode -> ArrivalAirport = flight.ArrivalAirport;
        linknode->next = nullptr;
        cur -> next = linknode;
        cur = cur -> next;
    }

    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据查询完成！用时：" << duration.count() << " 毫秒" << endl;
    cout << "ID为" << AirplaneID << "的飞机的执飞的航班顺序为：" << endl;
    AirplaneFlight *Cur = head -> next;
    while (Cur != nullptr){
        cout << "飞行编号：" << Cur->FlightID 
            << " ,于 " << fixed << setprecision(3) << Cur->DepartureTime << " 在 " << Cur->DepartureAirport << " Airport起飞 "
            << " ,于 " << fixed << setprecision(3) << Cur->ArrivalTime << " 在 " << Cur->ArrivalAirport << " Airport降落" << endl;
        Cur = Cur -> next;
    }
}

// 数据插入与删除：删除指定机型执行的航班
struct PointedAirplaneModel{
    Flight F;
    PointedAirplaneModel *next = nullptr;
};

bool isModel(const Flight flight, int AirplaneModel){
    return flight.AirplaneModel == AirplaneModel;
}

PointedAirplaneModel* RemovePlaneModel(vector <Flight> &f, int AirplaneModel){
    cout << "数据删除中..." << endl;
    auto start = high_resolution_clock::now(); 

    bool found = false;
    PointedAirplaneModel *head = new PointedAirplaneModel;
    PointedAirplaneModel *cur = head;
    for (const auto &flight : f){
        if (flight.AirplaneModel == AirplaneModel){
            found = true;
            PointedAirplaneModel *linknode = new PointedAirplaneModel;
            linknode -> F = flight;
            linknode->next = nullptr;
            cur -> next = linknode;
            cur = cur -> next;
        }
    }
    if (!found) {
        auto end = high_resolution_clock::now(); 
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "不存在型号为 " << AirplaneModel << "的飞机" << endl;
        cout << "用时：" << duration.count() << " 毫秒" << endl;
        delete head;
        return nullptr;
        //cout << "数据删除完成！用时：" << duration.count() << " 毫秒" << endl;
    }
    auto newEnd = remove_if(f.begin(), f.end(), 
                           [AirplaneModel](const Flight& flight) {
                               return flight.AirplaneModel == AirplaneModel;
                           });
    
    f.erase(newEnd, f.end());
    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据删除完成！用时：" << duration.count() << " 毫秒" << endl;
    PointedAirplaneModel *cur1 = head -> next;
    cout << "型号为" << AirplaneModel << "执飞的航班ID有：" << endl;
    while (cur1 != nullptr){
        cout << cur1 -> F.FlightID << endl;
        cur1 = cur1 -> next;
    }
    return head;
}

// 数据插入与删除：对同一对启航、抵达机场，按照航程时长升序排列
PointedAirplaneModel *SortPointedModel(PointedAirplaneModel *head){
    cout << "数据排序中..." << endl;
    auto start = high_resolution_clock::now(); 

    vector <Flight> flight;
    PointedAirplaneModel *cur = head -> next;
    while (cur != nullptr){
        flight.push_back(cur -> F);
        cur = cur -> next;
    }
    sort (flight.begin(), flight.end(), CompareFlight);
    PointedAirplaneModel *Head = new PointedAirplaneModel;
    PointedAirplaneModel *Cur = Head;
    for (const auto &f : flight){ 
        PointedAirplaneModel *linknode = new PointedAirplaneModel;
        linknode -> F = f;
        linknode->next = nullptr;
        Cur -> next = linknode;
        Cur = Cur -> next;
    }
    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据排序完成！用时：" << duration.count() << " 毫秒" << endl;
    cout << "排序后的该机型的航班ID为：" << endl;
    for (const auto &f : flight){
        cout << f.FlightID << endl;
    }
    return Head;
}

// 数据插入与删除：对于新的链表，查找基础费用为素数的所有航班
void SearchPrimeFareInLink(PointedAirplaneModel *head){
    vector <Flight> flight;
    PointedAirplaneModel *cur = head -> next;
    while (cur != nullptr){
        flight.push_back(cur -> F);
        cur = cur -> next;
    }
    SearchPrimeFare(flight);
}

// 数据插入与删除：判断鞍点是否存在
void SearchSaddlePointFlightInLink(PointedAirplaneModel *head, const ll &DepartureID, const ll &ArrivalID){
    vector <Flight> flight;
    PointedAirplaneModel *cur = head -> next;
    while (cur != nullptr){
        flight.push_back(cur -> F);
        cur = cur -> next;
    }
    SearchSaddlePointFlight(flight, DepartureID, ArrivalID);
}

int main(){
    SetConsoleOutputCP(65001);
    SelectPrimeNumber();
    vector <Flight> F = ReadFile(Path);
    // cout << "=== Flight IDs ===" << endl;
    // for (auto f: F){
    //     cout << f.FlightID << endl;
    // }
    // cout << "=== End ===" << endl;
    SortData(F);
    int selection = 0;
    while (1){
        cout << "请选择您想实现的功能：" << endl
         << "1. 数据查询" << endl
         << "2. 数据插入与删除" << endl
         << "3. 退出程序" << endl
         << "您的选择：";
        cin >> selection;
        cout << endl;
        switch (selection){
            case 1:
                int selection1;
                while (1){
                    bool ShouldBreak = false;
                    cout << "请选择具体要实现的功能：" << endl
                         << "1. 查找基础费用为素数的所有航班" << endl
                         << "2. 判断是否存在鞍点航班" << endl
                         << "3. 特定ID航班的执飞顺序" << endl
                         << "4. 返回主菜单" << endl
                         << "您的选择：";
                    cin >> selection1;
                    switch (selection1){
                        case 1:
                            SearchPrimeFare(F);
                            break;
                        case 2:
                            ll DID, AID;
                            cout << "请输入启航机场ID：";
                            while (!(cin >> DID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();  // 清除错误状态
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            cout << "请输入抵达机场ID：";
                            while (!(cin >> AID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();  // 清除错误状态
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            SearchSaddlePointFlight(F, DID, AID);
                            break;
                        case 3:
                            ll ID;
                            cout << "请输入飞机ID；";
                            while (!scanf("%lld", &ID)){
                                cout << "请输入正确的数据格式（整数）！" << endl;
                            }
                            cout << endl;
                            SearchAirPlaneFlightOrder(F, ID);
                            break;
                        case 4:
                            ShouldBreak = true;
                            break;
                        default:
                            cout << "无效的功能选择！请重新输入！" << endl;
                            break;
                    }
                    if (ShouldBreak) break;
                }
                break;
            case 2:
                int selection2;
                
                while (1){
                    static PointedAirplaneModel *head = nullptr;
                    bool ShouldBreak = false;
                    cout << "请选择您想实现的功能：" << endl
                        << "1. 删除特定机型执行的航班" << endl
                        << "2. 对删除的数据进行排序（请先执行1）" << endl
                        << "3. 在删除的数据中，查找基础费用为素数的航班（请先执行1）" << endl
                        << "4. 在删除的数据中，查找鞍点航班ID（请先执行1）" << endl
                        << "5. 返回主菜单" << endl
                        << "您的选择：";
                    cin >> selection2;
                    switch (selection2){
                        case 1:
                            int Model;
                            cout << "请输入飞机型号（1 - 5）：";
                            while (!scanf("%d", &Model) || (Model != 1 && Model != 2 && Model != 3 && Model != 4 && Model != 5)){
                                cout << "请输入正确的飞机型号（1 - 5）！" << endl;
                            }
                            cout << endl;
                            head = RemovePlaneModel(F, Model);
                            // cout << endl;
                            // cout << head << endl;
                            break;
                        case 2:
                            if (head == nullptr){
                                cout << "请先删除特定机型执行的航班！"  << endl;
                                break;
                            }
                            head = SortPointedModel(head);
                            break;
                        case 3:
                            if (head == nullptr){
                                cout << "请先删除特定机型执行的航班！"  << endl;
                                break;
                            }
                            SearchPrimeFareInLink(head);
                            break;
                        case 4:
                            if (head == nullptr){
                                cout << "请先删除特定机型执行的航班！"  << endl;
                                break;
                            }
                            ll DID, AID;
                            cout << "请输入启航机场ID：";
                           while (!(cin >> DID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();  // 清除错误状态
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // 清空缓冲区
                            }
                            cout << "请输入抵达机场ID：";
                            while (!(cin >> AID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();  // 清除错误状态
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // 清空缓冲区
                            }
                            SearchSaddlePointFlightInLink(head, DID, AID);
                            break;
                        case 5:
                            ShouldBreak = true;
                            break;
                        default:
                            cout << "无效的功能选择！请重新输入！" << endl;
                            break;
                    }
                    if (ShouldBreak) break;
                }
                break;
            case 3:
                return 0;
            default:
                cout << "无效的功能选择！请重新输入！" << endl;
                break;
        }
    }
    return 0;
}

