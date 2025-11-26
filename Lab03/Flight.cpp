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
#include <unordered_map>
#include <ctime>
#include <sstream>

using namespace std;
using namespace std::chrono;   //用于⏲计时
#define ll long long
#define inf 0x3ffff

//文件路径
const char *Path = "D:\\vscode_cpp\\Algorithm\\Lab02\\flight01.csv";
const char *NewPath = "D:\\vscode_cpp\\Algorithm\\Lab02\\flight-data.csv"; //区别仅在于日期格式不同。

struct Flight{
    ll FlightID;                //航班ID
    ll DepartureDate;           //启航日期
    bool IsIntl;                //航班类型（国际1/国内0）
    string FlightNo;            //航班编号
    ll DepartureAirport;        //启航机场ID
    ll ArrivalAirport;          //抵达机场ID
    double DepartureTime;       //启航时间
    double ArrivalTime;         //抵达时间
    ll AirplaneID;              //飞机ID
    int AirplaneModel;          //飞机机型
    ll AirFares;                //基础费用
    ll PeakSeasonRates;         //旺季价格
    ll OffSeasonRates;          //淡季价格
    bool IsSuspended = false;   //是否暂停
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
        if (s < '0' || s > '9'){
            //cout << "What you input is NOT a number! Please check the value or the position!" << endl;
            return -1;
        }
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
        if (( s < '0' || s > '9') && s != '.') {
            //cout << "What you input is NOT a number! Please check the value or the position!" << endl;
            return -1;
        }
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

//日期转换函数
static int64_t days_from_civil(int y, unsigned m, unsigned d) noexcept {
    y -= m <= 2;
    const int64_t era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);            // [0, 399]
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;   // [0, 365]
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;           // [0, 146096]
    return era * 146097 + static_cast<int64_t>(doe) - 719468;             // 719468 是 1970-01-01 的偏移修正
}

// 解析 "M/D/YYYY" 或 "M/D/YYYY H:M"（单/双位都支持），返回 Excel 序列号（天数，包含小数）
// 解析失败返回 -1.0
double excelDateTimeToSerial_Modern(const string &dateStr) {
    if (dateStr.empty()) return -1.0;

    // 去掉首尾空白和可能的 '\r'
    size_t s = 0;
    while (s < dateStr.size() && isspace((unsigned char)dateStr[s])) ++s;
    size_t e = dateStr.size();
    while (e > s && isspace((unsigned char)dateStr[e-1])) --e;
    string t = dateStr.substr(s, e - s);

    // 尝试用 sscanf 解析：允许 "m/d/yyyy" 或 "m/d/yyyy H:M"
    int month=0, day=0, year=0, hour=0, min=0;
    int n = sscanf(t.c_str(), "%d/%d/%d %d:%d", &month, &day, &year, &hour, &min);
    if (n < 3) return -1.0; // 解析失败（至少要有 m/d/yyyy）

    if (month < 1 || month > 12) return -1.0;
    if (day < 1 || day > 31) return -1.0;
    if (year < 0) return -1.0; // 年份必须合理（可根据需要放宽）

    if (n == 3) { hour = 0; min = 0; } // 只有日期，无时间
    else if (n == 5) {
        if (hour < 0 || hour > 23) return -1.0;
        if (min < 0 || min > 59) return -1.0;
    } else {
        // 有奇怪的解析结果（例如只解析到了小时但没分钟），当作失败
        return -1.0;
    }

    // 验证每月天数（简单校验，处理闰年）
    static const int mdays[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    int daysInMonth = mdays[month-1];
    bool isLeap = ( (year%4==0 && year%100!=0) || (year%400==0) );
    if (month == 2 && isLeap) daysInMonth = 29;
    if (day > daysInMonth) return -1.0;

    // 计算天数差：days(date) - days(1899-12-30)
    int64_t days_date = days_from_civil(year, (unsigned)month, (unsigned)day);
    int64_t days_excel_epoch = days_from_civil(1899, 12, 30);
    double days_diff = static_cast<double>(days_date - days_excel_epoch);

    // 加入当日内的小数部分
    double frac = (hour * 3600.0 + min * 60.0) / 86400.0;

    return days_diff + frac;
}
//按逗号分割字符串
vector<string> SplitByComma(string str) {
    vector<string> tokens;
    stringstream ss(str);
    string item;
    while (getline(ss, item, ',')) {
        // 去除可能的首尾空格
        item.erase(0, item.find_first_not_of(" "));
        item.erase(item.find_last_not_of(" ") + 1);
        tokens.push_back(item);
    }
    return tokens;
}

// 保存数据
void SaveFile(const char *Path, const vector <Flight> &flights){
    FILE *file = fopen(Path, "w");
    if (file == NULL) {
        cout << "无法打开文件进行写入: " << Path << endl;
        return;
    }
    // 写入标题行
    fprintf(file, "Flight ID,Departure date,Intl/Dome,Flight No.,Departure airport,Arrival airport,Departure Time,Arrival Time,Airplane ID,Airplane Model,Air fares,Peak-season fares,Off-season rates\n");

    for (const auto &f : flights) {
        // IsIntl 转回字符串
        const char* intlStr = f.IsIntl ? "Intl" : "Dome";
        fprintf(file, "%lld,%lld,%s,%s,%lld,%lld,%.6lf,%.6lf,%lld,%d,%lld,%lld,%lld\n",
            f.FlightID, f.DepartureDate, intlStr, f.FlightNo.c_str(),
            f.DepartureAirport, f.ArrivalAirport, f.DepartureTime, f.ArrivalTime,
            f.AirplaneID, f.AirplaneModel, f.AirFares, f.PeakSeasonRates, f.OffSeasonRates);
    }
    fclose(file);
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
        if (!PrimeNumberTable[flight.AirFares] && !flight.IsSuspended){
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
        if (!f[i].IsSuspended){
            line.push_back(f[i].AirFares);
            line.push_back(f[i].PeakSeasonRates);
            line.push_back(f[i].OffSeasonRates);
            line.push_back(f[i].FlightID);
            Matrix.push_back(line);
        }
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
        if (flight.AirplaneID == AirplaneID && !flight.IsSuspended){
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

// 数据查询：直达航班
void SearchDirectFlight(vector <Flight> &f, const ll DepartureID, const ll &ArrivalID){
    cout << "数据查询中..." << endl;
    auto start = high_resolution_clock::now(); 

    vector <Flight> DirectFlights;
    for (const auto &flight : f){
        if (flight.DepartureAirport == DepartureID && flight.ArrivalAirport == ArrivalID && !flight.IsSuspended){
            DirectFlights.push_back(flight);
        }
    }
    /* 
        我只能理解你要求最便宜和最快的航班ID，这俩是∪的关系，但是必须和+1day是∩的关系；
        那你为什么不明确说？
        我有cheapest但是不是+1day，就不算了？你能不能说清楚？
        这是人啊？
        真不是给人看的要求。
    */
    ll CheapestID, FastestID, CheapestFare = inf;
    double FastestTime = inf;
    // bool cheapest_day = false, fastest_day = false;
    for (const auto &flight : DirectFlights){
        bool IsAddDay = false;
        if (int(flight.ArrivalTime) - (flight.DepartureDate) == 1){
            IsAddDay = true;
        }
        if (flight.AirFares < CheapestFare && IsAddDay == true){
            CheapestFare = flight.AirFares;
            CheapestID = flight.FlightID;
        }
        double duration = flight.ArrivalTime - flight.DepartureTime;
        if (duration < FastestTime && IsAddDay == true){
            FastestTime = duration;
            FastestID = flight.FlightID;
        }
    }
    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据查询完成！用时：" << duration.count() << " 毫秒" << endl;
    if (DirectFlights.empty()){
        cout << "不存在直达航班！" << endl;
        return;
    }
    cout << "直达航班的航班ID：" << endl;
    for (const auto &f : DirectFlights){
        cout << f.FlightID ;
        if (f.FlightID == CheapestID){
            if (f.FlightID == FastestID){
                cout << "(cheapest, shortest duration, +1 day)";
            }else cout << "(cheapest, +1 day)";
        }else if (f.FlightID == FastestID){
            cout << "(shortest duration, +1 day)";
        }
        cout << " ";
    }
    cout << endl;
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
        if (flight.AirplaneModel == AirplaneModel && !flight.IsSuspended){
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
        if (!f.IsSuspended)
        {
            PointedAirplaneModel *linknode = new PointedAirplaneModel;
            linknode -> F = f;
            linknode->next = nullptr;
            Cur -> next = linknode;
            Cur = Cur -> next;
        }
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



// 新增功能1：批量增加数据
void AddDataFromFile(const char *newPath, vector <Flight> &f){
    cout << "数据批量增加中..." << endl;
    auto start = high_resolution_clock::now(); 

    FILE *file = fopen(NewPath,"r");
    if (file == NULL){
        cout << "文件打开失败!" << endl;
        return;
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
        f.push_back(flight);
    }

    fclose(file);
    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据批量添加完成！用时：" << duration.count() << " 毫秒" << endl;

    return;
}

// 新增功能1：批量删除数据
void RemoveData(vector<Flight> &f, int input) {
    cout << "批量数据删除中..." << endl;
    auto start = high_resolution_clock::now();

    size_t originalSize = f.size();
    vector<Flight> removedFlights;

    switch (input) {
        case 1: { // 删除指定启航机场的所有航班
            ll airportID;
            cout << "请输入要删除的启航机场ID：";
            while (!(cin >> airportID)) {
                cout << "请输入正确的数据格式（整数）！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            
            auto newEnd = remove_if(f.begin(), f.end(),
                [airportID](const Flight& flight) {
                    return flight.DepartureAirport == airportID;
                });
            
            // 保存被删除的航班
            removedFlights.assign(newEnd, f.end());
            f.erase(newEnd, f.end());
            break;
        }
        case 2: { // 删除指定抵达机场的所有航班
            ll airportID;
            cout << "请输入要删除的抵达机场ID：";
            while (!(cin >> airportID)) {
                cout << "请输入正确的数据格式（整数）！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            
            auto newEnd = remove_if(f.begin(), f.end(),
                [airportID](const Flight& flight) {
                    return flight.ArrivalAirport == airportID;
                });
            
            removedFlights.assign(newEnd, f.end());
            f.erase(newEnd, f.end());
            break;
        }
        case 3: { // 删除国内或国际航班
            int flightType;
            cout << "请选择要删除的航班类型（0-国内, 1-国际）：";
            while (!(cin >> flightType) || (flightType != 0 && flightType != 1)) {
                cout << "请输入正确的航班类型（0或1）！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            
            auto newEnd = remove_if(f.begin(), f.end(),
                [flightType](const Flight& flight) {
                    return flight.IsIntl == flightType;
                });
            
            removedFlights.assign(newEnd, f.end());
            f.erase(newEnd, f.end());
            break;
        }
        case 4: { // 删除指定航班编号的所有航班
            string flightNo;
            cout << "请输入要删除的航班编号：";
            cin >> flightNo;
            
            auto newEnd = remove_if(f.begin(), f.end(),
                [flightNo](const Flight& flight) {
                    return flight.FlightNo == flightNo;
                });
            
            removedFlights.assign(newEnd, f.end());
            f.erase(newEnd, f.end());
            break;
        }
        case 5: { // 删除指定飞机ID的所有航班
            ll airplaneID;
            cout << "请输入要删除的飞机ID：";
            while (!(cin >> airplaneID)) {
                cout << "请输入正确的数据格式（整数）！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            
            auto newEnd = remove_if(f.begin(), f.end(),
                [airplaneID](const Flight& flight) {
                    return flight.AirplaneID == airplaneID;
                });
            
            removedFlights.assign(newEnd, f.end());
            f.erase(newEnd, f.end());
            break;
        }
        case 6: { // 删除指定机型的所有航班
            int airplaneModel;
            cout << "请输入要删除的飞机型号（1-5）：";
            while (!(cin >> airplaneModel) || airplaneModel < 1 || airplaneModel > 5) {
                cout << "请输入正确的飞机型号（1-5）！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            
            auto newEnd = remove_if(f.begin(), f.end(),
                [airplaneModel](const Flight& flight) {
                    return flight.AirplaneModel == airplaneModel;
                });
            
            removedFlights.assign(newEnd, f.end());
            f.erase(newEnd, f.end());
            break;
        }
        default:
            cout << "无效的删除选项！" << endl;
            return;
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    size_t removedCount = originalSize - f.size();
    cout << "批量数据删除完成！用时：" << duration.count() << " 毫秒" << endl;
    cout << "共删除了 " << removedCount << " 条航班记录" << endl;
    
    if (removedCount > 0) {
        cout << "被删除的航班ID：" << endl;
        for (const auto& flight : removedFlights) {
            cout << flight.FlightID << endl;
        }
    } else {
        cout << "未找到符合条件的航班记录" << endl;
    }
}

// 新增功能2：暂停和恢复航班
void SuspendFlight(vector <Flight> &f, ll AirportID){
    cout << "数据处理中..." << endl;
    auto start = high_resolution_clock::now();

    vector <Flight> flights;
    for (auto &flight : f){
        if (flight.DepartureAirport == AirportID || flight.ArrivalAirport == AirportID){
            flight.IsSuspended = true;
            flights.push_back(flight);
        }
    }

    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据处理完成！用时：" << duration.count() << " 毫秒" << endl;
    cout << "暂停的航班有：" << endl;
    for (const auto &f : flights){
        cout << f.FlightID << endl;
    }
}

void RestoreFlight(vector <Flight> &f, ll AirportID){
    cout << "数据处理中..." << endl;
    auto start = high_resolution_clock::now();

    vector <Flight> flights;
    for (auto &flight : f){
        if (flight.DepartureAirport == AirportID || flight.ArrivalAirport == AirportID){
            flight.IsSuspended = false;
            flights.push_back(flight);
        }
    }

    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据处理完成！用时：" << duration.count() << " 毫秒" << endl;
    cout << "恢复的航班有：" << endl;
    for (const auto &f : flights){
        cout << f.FlightID << endl;
    }
}

//新增功能3：最多乘机问题
unordered_map<ll, vector<vector<ll>>> memo;  // 记忆化存储，存储所有最长路径

// 修改记忆化搜索函数，返回所有最长路径
vector<vector<ll>> findAllLongestPaths(const vector<Flight>& flights, ll currentFlightID, const Flight& currentFlight) {
    if (memo.count(currentFlightID)) {
        return memo[currentFlightID];
    }
    
    vector<vector<ll>> allPaths = {{currentFlightID}};
    int maxLength = 1;
    
    for (const auto& flight : flights) {
        if (flight.DepartureTime >= currentFlight.ArrivalTime + 0.020833 &&!flight.IsSuspended 
            && flight.DepartureAirport == currentFlight.ArrivalAirport) {
            
            vector<vector<ll>> subPaths = findAllLongestPaths(flights, flight.FlightID, flight);
            
            // 如果找到更长的路径，重置所有路径
            if (!subPaths.empty() && subPaths[0].size() + 1 > maxLength) {
                maxLength = subPaths[0].size() + 1;
                allPaths.clear();
                for (const auto& path : subPaths) {
                    vector<ll> newPath = {currentFlightID};
                    newPath.insert(newPath.end(), path.begin(), path.end());
                    allPaths.push_back(newPath);
                }
            }
            // 如果路径长度相同，添加到结果中
            else if (!subPaths.empty() && subPaths[0].size() + 1 == maxLength) {
                for (const auto& path : subPaths) {
                    vector<ll> newPath = {currentFlightID};
                    newPath.insert(newPath.end(), path.begin(), path.end());
                    allPaths.push_back(newPath);
                }
            }
        }
    }
    
    memo[currentFlightID] = allPaths;
    return allPaths;
}

void MaxFlightTaking(const vector<Flight> &f, ll StartFlightID, Flight StartFlight) {
    cout << "查找最多乘机航线中..." << endl;
    auto start = high_resolution_clock::now();

    // 清空记忆化存储
    memo.clear();
    
    // 查找所有最长路径
    vector<vector<ll>> allLongestPaths = findAllLongestPaths(f, StartFlightID, StartFlight);

    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "数据处理完成！用时：" << duration.count() << " 毫秒" << endl;
    
    if (allLongestPaths.empty() || allLongestPaths[0].size() <= 1) {
        cout << "不存在合适的航线！" << endl;
        return;
    }
    
    int pathLength = allLongestPaths[0].size();
    cout << "找到 " << allLongestPaths.size() << " 条最长航线（" << pathLength << "个航班）：" << endl;
    
    for (size_t i = 0; i < allLongestPaths.size(); i++) {
        cout << "航线 " << i+1 << ": ";
        for (size_t j = 0; j < allLongestPaths[i].size(); j++) {
            cout << allLongestPaths[i][j];
            if (j < allLongestPaths[i].size() - 1) {
                cout << " -> ";
            }
        }
        cout << endl;
    }
}


void InsertData(const string& input, vector<Flight> &F){
    string dataStr = input;
    size_t pos = dataStr.find("insert");
    if (pos != string::npos) {
        dataStr = dataStr.substr(pos + 6);
    }
    dataStr.erase(0, dataStr.find_first_not_of(" "));

    vector<string> tokens = SplitByComma(dataStr);

    if (tokens.size() != 11 && tokens.size() != 13) {
        cout << "输入字段数量错误（需要 11 或 13 项，用逗号分隔）。当前字段数：" << tokens.size() << endl;
        return;
    }

    Flight f;
    bool IsSuc = true; 

    // ID
    ll newID = ChangeToLong(tokens[0]);
    if (newID == -1) {
        cout << "航班ID 格式错误：" << tokens[0] << endl;
        return;
    }
    for (const auto& flight : F) {
        if (flight.FlightID == newID) {
            cout << "航班 ID " << newID << " 已存在！" << endl;
            return;
        }
    }
    f.FlightID = newID;

    // DepartureDate（如果你的 ChangeToLong 不能处理日期字符串，请改为专门的日期解析函数）
    f.DepartureDate = excelDateTimeToSerial_Modern(tokens[1]);
    if (f.DepartureDate == -1) {
        cout << "出发日期格式错误：" << tokens[1] << endl;
        IsSuc = false;
    }

    // 类型
    string type = tokens[2];
    if (type == "Intl" || type == "intl" || type == "1") f.IsIntl = true;
    else if (type == "Dome" || type == "dome" || type == "0") f.IsIntl = false;
    else {
        cout << "航班类型字段错误（应为 Intl/Dome 或 1/0）：" << tokens[2] << endl;
        IsSuc = false;
    }

    f.FlightNo = tokens[3];

    f.DepartureAirport = ChangeToLong(tokens[4]);
    if (f.DepartureAirport == -1) {
        cout << "出发机场ID 格式错误：" << tokens[4] << endl;
        IsSuc = false;
    }
    f.ArrivalAirport = ChangeToLong(tokens[5]);
    if (f.ArrivalAirport == -1) {
        cout << "抵达机场ID 格式错误：" << tokens[5] << endl;
        IsSuc = false;
    }

    f.DepartureTime = excelDateTimeToSerial_Modern(tokens[6]);
    if (f.DepartureTime == -1) {
        cout << "出发时间格式错误：" << tokens[6] << endl;
        IsSuc = false;
    }
    f.ArrivalTime = excelDateTimeToSerial_Modern(tokens[7]);
    if (f.ArrivalTime == -1) {
        cout << "抵达时间格式错误：" << tokens[7] << endl;
        IsSuc = false;
    }

    f.AirplaneID = ChangeToLong(tokens[8]);
    if (f.AirplaneID == -1) {
        cout << "飞机ID 格式错误：" << tokens[8] << endl;
        IsSuc = false;
    }

    f.AirplaneModel = (int)ChangeToLong(tokens[9]);
    if (f.AirplaneModel < 1 || f.AirplaneModel > 5) {
        cout << "飞机型号取值应为 1..5（当前：" << tokens[9] << "）" << endl;
        IsSuc = false;
    }

    f.AirFares = ChangeToLong(tokens[10]);
    if (f.AirFares == -1) {
        cout << "基础票价格式错误：" << tokens[10] << endl;
        IsSuc = false;
    }

    if (tokens.size() == 11) {
        f.PeakSeasonRates = f.AirFares; // 默认等于基础票价
        f.OffSeasonRates = f.AirFares;  // 默认等于基础票价
    } else {
        f.PeakSeasonRates = ChangeToLong(tokens[11]);
        f.OffSeasonRates = ChangeToLong(tokens[12]);
        if (f.PeakSeasonRates == -1 || f.OffSeasonRates == -1) {
            cout << "季节票价格式错误：" << tokens[11] << "," << tokens[12] << endl;
            IsSuc = false;
        }
    }

    f.IsSuspended = false;

    // 最终检查并输出明确提示
    if (!IsSuc) {
        cout << "输入格式内容有误，请检查上面的具体错误提示并重试。" << endl;
    } else if (f.DepartureTime >= f.ArrivalTime) {
        cout << "错误：抵达时间早于或等于出发时间！" << endl;
    } else {
        F.push_back(f);
        SortData(F);
        cout << "数据插入成功！" << endl;
    }
}

//插入与更新数据：更新数据
void UpdateData(const string &input, vector<Flight> &F) {
    string dataPart = input;
    size_t pos = dataPart.find("update");
    if (pos == 0) {
        dataPart = dataPart.substr(6); // 去掉开头 "update"
    }
    // 去掉开头空白
    dataPart.erase(0, dataPart.find_first_not_of(" "));

    vector<string> tokens = SplitByComma(dataPart);

    if (tokens.size() != 11 && tokens.size() != 13) {
        cout << "输入字段数量错误（需要 11 或 13 项，用逗号分隔）。当前字段数：" << tokens.size() << endl;
        return;
    }

    // 解析目标 ID 并定位要更新的记录
    ll targetID = ChangeToLong(tokens[0]);
    if (targetID == -1) {
        cout << "目标航班ID 格式错误：" << tokens[0] << endl;
        return;
    }

    bool Found = false;
    int targetIndex = -1;
    for (int i = 0; i < (int)F.size(); ++i) {
        if (F[i].FlightID == targetID) {
            Found = true;
            targetIndex = i;
            break;
        }
    }
    if (!Found) {
        cout << "航班ID为 " << targetID << " 的航班不存在！" << endl;
        return;
    }

    // 以原数据为基础，逐字段覆盖（解析失败则回滚，不修改原数据）
    Flight tempF = F[targetIndex];
    bool IsSuc = true;

    // DepartureDate 使用 excelDateTimeToSerial_Modern 解析（与 InsertData 保持一致）
    double depDate = excelDateTimeToSerial_Modern(tokens[1]);
    if (depDate < 0) {
        cout << "出发日期格式错误：" << tokens[1] << endl;
        IsSuc = false;
    } else {
        tempF.DepartureDate = depDate;
    }

    // Intl/Dome 类型
    string type = tokens[2];
    if (type == "Intl" || type == "intl" || type == "1") tempF.IsIntl = true;
    else if (type == "Dome" || type == "dome" || type == "0") tempF.IsIntl = false;
    else {
        cout << "航班类型字段错误（应为 Intl/Dome 或 1/0）：" << tokens[2] << endl;
        IsSuc = false;
    }

    // FlightNo
    tempF.FlightNo = tokens[3];

    // Airports
    tempF.DepartureAirport = ChangeToLong(tokens[4]);
    if (tempF.DepartureAirport == -1) {
        cout << "出发机场ID 格式错误：" << tokens[4] << endl;
        IsSuc = false;
    }
    tempF.ArrivalAirport = ChangeToLong(tokens[5]);
    if (tempF.ArrivalAirport == -1) {
        cout << "抵达机场ID 格式错误：" << tokens[5] << endl;
        IsSuc = false;
    }

    // Times：使用 excelDateTimeToSerial_Modern（与 InsertData 保持一致）
    double depTime = excelDateTimeToSerial_Modern(tokens[6]);
    if (depTime < 0) {
        cout << "出发时间格式错误：" << tokens[6] << endl;
        IsSuc = false;
    } else tempF.DepartureTime = depTime;

    double arrTime = excelDateTimeToSerial_Modern(tokens[7]);
    if (arrTime < 0) {
        cout << "抵达时间格式错误：" << tokens[7] << endl;
        IsSuc = false;
    } else tempF.ArrivalTime = arrTime;

    // Airplane Info
    tempF.AirplaneID = ChangeToLong(tokens[8]);
    if (tempF.AirplaneID == -1) {
        cout << "飞机ID 格式错误：" << tokens[8] << endl;
        IsSuc = false;
    }

    tempF.AirplaneModel = (int)ChangeToLong(tokens[9]);
    if (tempF.AirplaneModel < 1 || tempF.AirplaneModel > 5) {
        cout << "飞机型号取值应为 1..5（当前：" << tokens[9] << "）" << endl;
        IsSuc = false;
    }

    // Fares
    tempF.AirFares = ChangeToLong(tokens[10]);
    if (tempF.AirFares == -1) {
        cout << "基础票价格式错误：" << tokens[10] << endl;
        IsSuc = false;
    }

    if (tokens.size() == 11) {
        // 若只给 11 项，保持与 InsertData 的行为：季节票价默认为基础票价
        tempF.PeakSeasonRates = tempF.AirFares;
        tempF.OffSeasonRates  = tempF.AirFares;
    } else {
        tempF.PeakSeasonRates = ChangeToLong(tokens[11]);
        tempF.OffSeasonRates  = ChangeToLong(tokens[12]);
        if (tempF.PeakSeasonRates == -1 || tempF.OffSeasonRates == -1) {
            cout << "季节票价格式错误：" << tokens[11] << "," << tokens[12] << endl;
            IsSuc = false;
        }
    }

    // 保留原 IsSuspended（tempF 已复制原值）

    // 最终校验
    if (!IsSuc) {
        cout << "数据格式解析错误，请检查输入并重试。" << endl;
        return;
    }
    if (tempF.DepartureTime >= tempF.ArrivalTime) {
        cout << "错误：抵达时间早于或等于出发时间！" << endl;
        return;
    }

    // 更新并排序
    tempF.FlightID = targetID; // 保证 ID 不变
    F[targetIndex] = tempF;
    SortData(F);
    cout << "数据更新成功！" << endl;
}
//菜单
int main(){
    SetConsoleOutputCP(65001);
    SelectPrimeNumber();
    vector <Flight> F = ReadFile(Path);
    SortData(F);
    int selection = 0;
    while (1){
        cout << "请选择您想实现的功能：" << endl
         << "1. 数据查询" << endl
         << "2. 数据删除与查看" << endl
         << "3. 批量增删数据" << endl
         << "4. 暂停与恢复航班" << endl
         << "5. 最多乘机问题" << endl
         << "6. 插入与更新数据" << endl
         << "7. 退出程序" << endl
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
                         << "4. 查询直达航班" << endl
                         << "5. 返回主菜单" << endl
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
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            cout << "请输入抵达机场ID：";
                            while (!(cin >> AID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            SearchSaddlePointFlight(F, DID, AID);
                            break;
                        case 3:
                            ll ID;
                            cout << "请输入飞机ID：";
                            while (!(cin >> ID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            SearchAirPlaneFlightOrder(F, ID);
                            break;
                        case 4:
                        {
                            cout << "请输入要查询的启航以及抵达机场ID：";
                            cin.get();
                            string input;
                            getline(cin, input);
                            stringstream ss(input);
                            ll DepartureID = 0, ArrivalID = 0;
                            ss >> DepartureID >> ArrivalID;
                            SearchDirectFlight(F, DepartureID, ArrivalID);
                            break;
                        }

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
                        << "5. 删除特定航班ID对应的航班" << endl
                        << "6. 返回主菜单" << endl
                        << "您的选择：";
                    cin >> selection2;
                    switch (selection2){
                        case 1:
                            int Model;
                            cout << "请输入飞机型号（1-5）：";
                            while (!(cin >> Model) || Model < 1 || Model > 5) {
                                cout << "请输入正确的飞机型号（1-5）！" << endl;
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            head = RemovePlaneModel(F, Model);
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
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            cout << "请输入抵达机场ID：";
                            while (!(cin >> AID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            SearchSaddlePointFlightInLink(head, DID, AID);
                            break;
                        case 5:
                        {
                            cout << "请输入希望删除的航班ID：";
                            ll ID;
                            cin >> ID;
                            // remove_if 把需要删除的元素移到末尾，返回新逻辑结尾的迭代器
                            auto newEnd = std::remove_if(F.begin(), F.end(), 
                                [ID](const Flight& f) {
                                    return f.FlightID == ID; // Lambda 表达式：条件满足则标记删除
                                });
                            if (newEnd != F.end()) {
                                F.erase(newEnd, F.end()); // 真的擦除末尾的垃圾数据
                                cout << "删除成功！" << endl;
                            } else {
                                cout << "未找到该航班ID。" << endl;
                            }
                            break;
                        }
                        case 6:
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
                int selection3;
                while (1){
                    bool ShouldBreak = false;
                    cout << "请选择具体要实现的功能：" << endl
                         << "1. 从文件增加数据" << endl
                         << "2. 批量删除数据" << endl
                         << "3. 返回主菜单" << endl
                         << "您的选择：";
                    cin >> selection3;
                    switch (selection3){
                        case 1:
                            AddDataFromFile(NewPath, F);
                            // 重新排序新增数据后的航班列表
                            SortData(F);
                            break;
                        case 2:
                            int deleteOption;
                            cout << "请选择您想删除的数据特征：" << endl
                                 << "1. 启航机场" << endl
                                 << "2. 抵达机场" << endl
                                 << "3. 国内/国际" << endl
                                 << "4. 航班编号" << endl
                                 << "5. 飞机ID" << endl
                                 << "6. 飞机型号" << endl
                                 << "7. 返回上一级菜单" << endl
                                 << "您的选择：";
                            while (!(cin >> deleteOption) || deleteOption < 1 || deleteOption > 7) {
                                cout << "请输入正确的选项（1-7）！" << endl;
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            if (deleteOption != 7) {
                                RemoveData(F, deleteOption);
                                // 重新排序删除数据后的航班列表
                                SortData(F);
                            }
                            break;
                        case 3:
                            ShouldBreak = true;
                            break;
                        default:
                            cout << "无效的功能选择！请重新输入！" << endl;
                            break;
                    }
                    if (ShouldBreak) break;
                }
                break;
            case 4:
                int selection4;
                while (1){
                    bool ShouldBreak = false;
                    cout << "请选择具体要实现的功能：" << endl
                         << "1. 暂停航班" << endl
                         << "2. 恢复航班" << endl
                         << "3. 返回主菜单" << endl
                         << "您的选择：";
                    cin >> selection4;
                    switch (selection4){
                        case 1:
                            ll suspendAirportID;
                            cout << "请输入要暂停的机场ID：";
                            while (!(cin >> suspendAirportID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            SuspendFlight(F, suspendAirportID);
                            break;
                        case 2:
                            ll restoreAirportID;
                            cout << "请输入要恢复的机场ID：";
                            while (!(cin >> restoreAirportID)) {
                                cout << "请输入正确的数据格式（整数）！" << endl;
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            RestoreFlight(F, restoreAirportID);
                            break;
                        case 3:
                            ShouldBreak = true;
                            break;
                        default:
                            cout << "无效的功能选择！请重新输入！" << endl;
                            break;
                    }
                    if (ShouldBreak) break;
                }
                break;
            case 5: 
                {ll startFlightID = -1;
                cout << "请输入起始航班ID：";
                while (!(cin >> startFlightID)) {
                    cout << "请输入正确的数据格式（整数）！" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                
                // 查找起始航班信息
                Flight flight1;
                bool found = false;
                for (const auto &f : F){
                    if (f.FlightID == startFlightID && !f.IsSuspended){
                        found = true;
                        flight1 = f;
                        break;
                    }
                }
                if (!found) {
                    cout << "未找到该航班ID或该航班已暂停！" << endl;
                    break;  
                }
                MaxFlightTaking(F, flight1.FlightID, flight1);
                break;
            }
            case 6:
                int selection6;
                while (1){
                    bool ShouldBreak = false;
                    cout << "请选择具体要实现的功能：" << endl
                         << "1. 插入数据" << endl
                         << "2. 更新数据" << endl
                         << "3. 返回主菜单" << endl
                         << "您的选择：";
                    cin >> selection6;
                    switch(selection6){
                        case 1:
                        {
                            cout << "请按照顺序输入数据：" ;
                            string input;
                            cin.get();
                            getline(cin, input);
                            InsertData(input, F);
                            break;
                        }
                        case 2:
                        {
                            cout << "请输入你要修改的航班信息：";
                            string input;
                            cin.get();
                            getline(cin, input);
                            UpdateData(input, F);
                            break;
                        }
                        case 3:
                            ShouldBreak  = true;
                            break;
                        default:
                            cout << "无效的功能选择！请重新输入！" << endl;
                            break;
                    }
                    if (ShouldBreak) break;
                }
            case 7:
                return 0;
            default:
                cout << "无效的功能选择！请重新输入！" << endl;
                break;
        }
        cout << endl;
    }
    SaveFile(Path, F);
    return 0;
}