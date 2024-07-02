#include <bits/stdc++.h>
#include <Windows.h>
using namespace std;

vector<vector<uint32_t>> arrays;
set<uint32_t> res;
vector<bool> Flag;

bool compareBySize(uint32_t& a, uint32_t& b) {
    return arrays[a].size() != arrays[b].size() ? arrays[a].size() < arrays[b].size() : a < b;
}

int main() {
    ifstream file("ExpIndex", ios::binary);
    if (!file) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    uint32_t arrayLength;
    while (file.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength))) {
        std::vector<uint32_t> array(arrayLength);
        file.read(reinterpret_cast<char*>(array.data()), arrayLength * sizeof(uint32_t));
        arrays.push_back(array);
    }
    file.close();

    ifstream queryFile("ExpQuery");
    if (!queryFile) {
        cerr << "Failed to open the query file." << endl;
        return 1;
    }

    ofstream resultFile("Time");
    if (!resultFile) {
        cerr << "Failed to create the result file." << endl;
        return 1;
    }

    LARGE_INTEGER freq, start, end0;
    QueryPerformanceFrequency(&freq);

    string line;
    int queryCount = 1;
    int targetQuery;

    // 让用户输入查询编号
    cout << "请输入查询编号（1-1000之间的整数执行单条查询，其他整数执行所有查询，非整数向下取整）: ";
    cin >> targetQuery;

    while (getline(queryFile, line)) {
        if (targetQuery >= 1 && targetQuery <= 1000 && queryCount != targetQuery) {
            queryCount++;
            continue;
        }

        uint32_t k = 0;
        stringstream ss(line);
        vector<uint32_t> queryIndices;
        uint32_t row_idx;
        bool flag = true;

        while (ss >> row_idx) {
            queryIndices.push_back(row_idx);
            k++;
        }
        sort(queryIndices.begin(), queryIndices.end(), compareBySize);
        QueryPerformanceCounter(&start);       
        //测试框架！--------------------------------------------------------------------------------


        for (auto idx : queryIndices) {
            if (flag) {
                for (auto elem : arrays[idx]) {
                    res.insert(elem);
                    Flag.push_back(true);
                }
                flag = false;
            }
            else {
                vector<uint32_t> unfound;
                for (auto elem : res) {
                    bool flag0 = false;
                    for (size_t i = 0; i < arrays[idx].size(); i++) {
                        if (elem == arrays[idx][i]) {
                            flag0 = true;
                            break;
                        }
                    }
                    if (!flag0) {
                        unfound.push_back(elem);
                    }
                }
                for (size_t i = 0; i < unfound.size(); i++) {
                    res.erase(unfound[i]);
                }
            }
        }



        //-----------------------------------------------------------------------------------
        QueryPerformanceCounter(&end0);
        double elapsedSeconds = static_cast<double>(end0.QuadPart - start.QuadPart) / freq.QuadPart;
        cout << "Query " << queryCount << ": " << elapsedSeconds << " seconds" << endl;
        cout << "Result size: " << res.size() << endl;
        //resultFile<< elapsedSeconds<< endl;
        if (targetQuery >= 1 && targetQuery <= 1000) {
            break; // 如果是单条查询，执行后跳出循环
        }

        res.clear();
        Flag.clear();
        queryCount++;
    }
    queryFile.close();
    resultFile.close();
    return 0;
}
