#include "aoc.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <array>
#include <map>
#include <queue>
#include <set>

std::tuple<std::string, std::string> p19(const std::string & input) {
    int64_t ans1 = 0;
    int64_t ans2 = 0;
    std::vector<std::vector<std::array<int,3>>> v;

    {
        int num = 0;
        bool havenum = false;
        bool neg = false;
        std::vector<std::vector<int>> tmp;
        bool newblock = false;
        for(const auto c : input) {
            if(c >= '0' && c <= '9') {
                num *= 10;
                num += c-'0';
                havenum = true;
            } else if(c == '-') {
                neg = true;
            } else if(c == 'r') {
                newblock = true;
            } else {
                if(havenum) {
                    if(newblock) {
                        tmp.emplace_back();
                        newblock = false;
                    }
                    tmp.back().push_back(neg ? -num : num);
                }
                neg = false;
                havenum = false;
                num = 0;
            }

        }
        for(auto && q : tmp) {
            v.emplace_back();
            for(size_t i = 1; i < q.size(); i+=3) {
                v.back().push_back({q[i],q[i+1],q[i+2]});
            }
        }
    }

    auto transform = [](const std::array<int,3> & a, const std::array<int,3> & p, int num) -> std::array<int, 3> {
        bool n1 = num & 1;
        bool n2 = num & 2;
        bool n3 = num & 4;
        return { n1 ? -a[p[0]] : a[p[0]],n2 ? -a[p[1]] : a[p[1]],n3 ? -a[p[2]] : a[p[2]] };
    };

    std::vector<std::tuple<size_t,size_t,std::array<int,3>,int,std::tuple<int,int,int>>> overlaps;

    for (size_t s1 = 0; s1 < v.size(); s1++){
        for (size_t s2 = 0; s2 < v.size(); s2++) {
            if(s1 == s2) continue;
            std::array<int, 3> p{0, 1, 2};
            do {
                for (int n = 0; n < 8; n++) {
                    std::map<std::tuple<int, int, int>, int> m;
                    for (size_t i = 0; i < v[s2].size(); i++) {
                        auto a = transform(v[s2][i], p, n);
                        //std::cout << a[0] << a[1] << a[2] << '\n';

                        for (size_t j = 0; j < v[s1].size(); j++) {
                            m[{v[s1][j][0] - a[0], v[s1][j][1] - a[1], v[s1][j][2] - a[2]}]++;
                        }
                    }
                    for (auto &[t, i]: m) {
                        if (i >= 12) {
                            overlaps.emplace_back(s1,s2,p,n,t);
                            auto [ox,oy,oz] = t;
                            //std::cout << s1 << s2 << " found overlap i = " << i << ' ' << ox << ' ' << oy << ' ' << oz << std::endl;

                        }
                    }
                }

            } while (std::next_permutation(p.begin(), p.end()));

        }
    }


    {
        std::queue<std::tuple<size_t, size_t, std::vector<std::array<int, 3>>>> q;
        for (size_t i = 0; i < v.size(); i++) q.emplace(i, i, v[i]);
        std::map<std::tuple<int, int, int>, int> beacons;
        std::set<std::tuple<size_t, size_t>> seen;
        std::set<int> done;
        while (!q.empty()) {
            auto[s, orig, l] = std::move(q.front());
            q.pop();
            if (s == 0) {
                done.insert(orig);
                for (auto &a: l) {
                    beacons[{a[0], a[1], a[2]}]++;
                }
            } else {
                // find transform
                bool found = false;
                for (auto &[to, from, p, n, offset]: overlaps) {
                    if (seen.count({orig, to})) continue;
                    if (from == s) {
                        //std::cout << "transform " << from << " to " << to << " orig " << orig << std::endl;
                        std::vector<std::array<int, 3>> newvector;
                        auto[ox, oy, oz] = offset;
                        for (auto &beacontmp: l) {
                            auto beacontrans = transform(beacontmp, p, n);
                            newvector.push_back({beacontrans[0] + ox, beacontrans[1] + oy, beacontrans[2] + oz});
                        }
                        seen.insert({orig, s});
                        found = true;
                        q.emplace(to, orig, std::move(newvector));
                    }
                }
                //if (!found) std::cout << "no transform for " << s << " orig = " << orig << std::endl;
            }
        }

        /*
        for (auto &[pos, cnt]: beacons) {
            auto[x, y, z] = pos;
            std::cout << x << ' ' << y << ' ' << z << ':' << cnt << '\n';
        }
        */

        ans1 = beacons.size();
    }

    {
        std::vector<std::tuple<int,int,int>> scannerpos(v.size());
        std::vector<bool> scannerseen(v.size());
        std::queue<std::tuple<size_t,size_t,std::tuple<int,int,int>>> q;
        for (size_t i = 0; i < v.size(); i++) q.emplace(i, i, std::make_tuple(0,0,0));

        while(!q.empty()) {
            auto [scanidx,insys,pos] = q.front();
            q.pop();
            if(insys == 0) {
                scannerseen[scanidx] = true;
                scannerpos[scanidx] = pos;
            } else if(!scannerseen[scanidx]) {
                auto [x,y,z] = pos;
                for (auto &[to, from, p, n, offset]: overlaps) {
                    if(insys == from) {
                        auto[ox, oy, oz] = offset;
                        auto tp = transform({x,y,z}, p, n);
                        q.emplace(scanidx, to, std::make_tuple(tp[0]+ox,tp[1]+oy,tp[2]+oz));
                    }
                }
            }
        }


        for(auto [x1,y1,z1] : scannerpos) {
            for(auto [x2,y2,z2] : scannerpos) {
                ans2 = std::max(ans2, decltype(ans2)(std::abs(x1-x2)+std::abs(y1-y2)+std::abs(z1-z2)));
            }
        }
    }

    return {std::to_string(ans1), std::to_string(ans2)};
}
