#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

long long operationCount = 0;   
int maxDepth = 0;               

void complexRec(int n, int currentDepth) {
    maxDepth = max(maxDepth, currentDepth);
    operationCount++; 
    if (n <= 2) {
        operationCount++; 
        return;
    }
    int p = n;
    operationCount++;

    while (p > 0) {
        operationCount++; 

        vector<int> temp(n);
        operationCount += n; 

        for (int i = 0; i < n; i++) {
            temp[i] = i ^ p;
            operationCount += 2; 
        }

        p >>= 1;
        operationCount++; 
    }
    vector<int> small(n);
    operationCount += n; 

    for (int i = 0; i < n; i++) {
        small[i] = i * i;
        operationCount += 2; 

    reverse(small.begin(), small.end());
    operationCount += n; 
    complexRec(n / 2, currentDepth + 1);
    complexRec(n / 2, currentDepth + 1);
    complexRec(n / 2, currentDepth + 1);
}

int main() {
    int n = 1024;  

    auto start = high_resolution_clock::now();

    complexRec(n, 1);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Input size (n): " << n << endl;
    cout << "Total operations performed: " << operationCount << endl;
    cout << "Maximum recursion depth: " << maxDepth << endl;
    cout << "Execution time: " << duration.count() << " microseconds" << endl;

    return 0;
}
//recurrence relation T(n) = 3T(n/2) + O(n log n)
//asymptotic complexity using master theorem is O(n^(log2(3))) = O(n^1.585)
//depth of recursion tree is log2(n)
