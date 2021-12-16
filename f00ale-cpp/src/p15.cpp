#include "aoc.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <tuple>
#include <limits>

std::tuple<std::string, std::string> p15(const std::string & input) {
    int64_t ans1 = 0;
    int64_t ans2 = 0;
    std::vector<std::string> v;

    {
        bool first = true;
        for(const auto c : input) {
            if(c >= '0' && c <= '9') {
                if(first) v.emplace_back();
                first = false;
                v.back().push_back(c-'0');
            } else {
                if(c=='\n') first = true;
            }

        }
    }


    for(auto p : {1,2}) {
        if(p == 2) {
            decltype(v) n(5*v.size(), std::string(5*v[0].size(), 0));
            for(int dy = 0; dy < 5; dy++) {
                for(int dx = 0; dx < 5; dx++) {
                    for(size_t y = 0; y < v.size(); y++) {
                        for(size_t x = 0; x < v[y].size(); x++) {
                            auto tmp = v[y][x]+dx+dy;
                            while(tmp >= 10) tmp-=9;
                            n[y+dy*v.size()][x+dx*v[y].size()] = tmp;
                        }
                    }
                }
            }
            v.swap(n);
        }
        std::vector<std::vector<int>> risks(v.size(), std::vector<int>(v[0].size(), std::numeric_limits<int>::max()));

        using type = std::tuple<int,int,int>;
        std::priority_queue<type, std::vector<type>, std::greater<>> q;
        q.emplace(0, 0, 0);
        while (!q.empty()) {
            auto[w, y, x] = q.top();
            q.pop();
            if (w < risks[y][x]) {
                risks[y][x] = w;
                if (y - 1 >= 0) q.emplace(w + v[y - 1][x], y - 1, x);
                if (y + 1 < static_cast<int>(v.size())) q.emplace(w + v[y + 1][x], y + 1, x);
                if (x - 1 >= 0) q.emplace(w + v[y][x - 1], y, x - 1);
                if (x + 1 < static_cast<int>(v[0].size())) q.emplace(w + v[y][x + 1], y, x + 1);
            }
        }

        (p == 1 ? ans1 : ans2)  = risks.back().back();
    }
    return {std::to_string(ans1), std::to_string(ans2)};
}