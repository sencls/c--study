#include <iostream>
#include <vector>
#include <utility>
using namespace std;
int main()
{
    int n, m;
    cin >> n >> m;
    int t = m;
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    vector<pair<int, int>> num(m);
    while (t--)
    {

        int x, y;
        cin >> x >> y;
        num[t].first = x;
        num[t].second = y;
    }
    for (int i = 1; i <= m; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            if (j >= num[i - 1].first)
                dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - num[i - 1].first] + num[i - 1].first * num[i - 1].second);
            else
                dp[i][j] = dp[i - 1][j];
        }
    }
    cout << dp[m][n];
    return 0;
}