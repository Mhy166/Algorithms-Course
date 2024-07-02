#include <bits/stdc++.h>
#include <Windows.h>
using namespace std;

vector<vector<uint32_t>> arrays;
set<uint32_t> res;

bool compareBySize(uint32_t& a, uint32_t& b) {
    return arrays[a].size() != arrays[b].size() ? arrays[a].size() < arrays[b].size() : a < b;
}

// 分治求交集函数
vector<uint32_t> intersect(const vector<uint32_t>& a, const vector<uint32_t>& b) {
    vector<uint32_t> result;
    auto it_a = a.begin();
    auto it_b = b.begin();

    while (it_a != a.end() && it_b != b.end()) {
        if (*it_a < *it_b) {
            ++it_a;
        }
        else if (*it_b < *it_a) {
            ++it_b;
        }
        else {
            result.push_back(*it_a);
            ++it_a;
            ++it_b;
        }
    }
    return result;
}

vector<uint32_t> divideAndConquerIntersection(const vector<uint32_t>& a, const vector<uint32_t>& b) {
    if (a.size() < 10 || b.size() < 10) {
        return intersect(a, b);
    }

    int mid_a = a.size() / 2;
    auto lower_b = lower_bound(b.begin(), b.end(), a[mid_a]);
    auto upper_b = upper_bound(b.begin(), b.end(), a[mid_a]);

    vector<uint32_t> left_a(a.begin(), a.begin() + mid_a);
    vector<uint32_t> right_a(a.begin() + mid_a + 1, a.end());
    vector<uint32_t> left_b(b.begin(), lower_b);
    vector<uint32_t> right_b(upper_b, b.end());

    vector<uint32_t> left_intersect = divideAndConquerIntersection(left_a, left_b);
    vector<uint32_t> right_intersect = divideAndConquerIntersection(right_a, right_b);

    vector<uint32_t> mid_intersect;
    if (lower_b != b.end() && *lower_b == a[mid_a]) {
        mid_intersect.push_back(a[mid_a]);
    }

    left_intersect.insert(left_intersect.end(), mid_intersect.begin(), mid_intersect.end());
    left_intersect.insert(left_intersect.end(), right_intersect.begin(), right_intersect.end());

    return left_intersect;
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
        
        while (ss >> row_idx) {
            queryIndices.push_back(row_idx);
            k++;
        }
        sort(queryIndices.begin(), queryIndices.end(), compareBySize);

        QueryPerformanceCounter(&start); 

        // 使用分治策略求交集
        if (queryIndices.size() > 1) {
            vector<uint32_t> result = arrays[queryIndices[0]];
            for (size_t i = 1; i < queryIndices.size(); ++i) {
                result = divideAndConquerIntersection(result, arrays[queryIndices[i]]);
            }
            res.insert(result.begin(), result.end());
        }
        else if (queryIndices.size() == 1) {
            res.insert(arrays[queryIndices[0]].begin(), arrays[queryIndices[0]].end());
        }

        QueryPerformanceCounter(&end0);
        double elapsedSeconds = static_cast<double>(end0.QuadPart - start.QuadPart) / freq.QuadPart;
        //resultFile << elapsedSeconds << endl;
        cout << "Query " << queryCount << ": " << elapsedSeconds << " seconds" << endl;
        cout << "Result size: " << res.size() << endl;
        if (targetQuery >= 1 && targetQuery <= 1000) {
            break; // 如果是单条查询，执行后跳出循环
        }

        res.clear();
        queryCount++;
    }
    queryFile.close();
    resultFile.close();
    return 0;
}
