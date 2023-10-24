//
// Created by saahi on 10/24/2023.
//
#include <bits/stdc++.h>
using namespace std;

class Info{

public:

    map<string,int> nodeInfoMap(map<string,int> &followers){

        followers.insert({"127.0.0.1",9910});
        followers.insert({"127.0.0.1" , 9911});
        followers.insert({"127.0.0.1" , 9912});
        followers.insert({"127.0.0.1" , 9913});

        return followers;
    }

};

