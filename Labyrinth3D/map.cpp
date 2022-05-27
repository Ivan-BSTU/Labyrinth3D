#include <cstdlib>
#include <cassert>

#include "map.h"
#include <iostream>
#include "player.h"

using namespace std;


    char map[] ="33333333333333333"\
                "3      3        3"\
                "3 3 333333 333333"\
                "3               3"\
                "3  3333 333333333"\
                "3  3            3"\
                "3  33333 333333 3"\
                "3   3     3 333 3"\
                "3   333333333   3"\
                "3   33      33333"\
                "3             333"\
                "3          33   3"\
                "3            3  3"\
                "3  3333    333 33"\
                "3     3333 333333"\
                "3               4"\
                "33333333333333333"; // ������� �����


// 17x17 ����� ��� ���������
Map::Map() : w(17), h(17) {
    assert(sizeof(map) == w * h + 1); // +1 for the null terminated string
    
}   
    
// �������� ������ �����, �������������� �����, ������������ �����
int Map::get(const size_t i, const size_t j) const{
    assert(i < w && j < h && sizeof(map) == w * h + 1);
     
    return map[i + j * (w)] - '0';
}   
    
// �������� �� ���������� �����
bool Map::is_empty(const size_t i, const size_t j) const {
    assert(i < w && j < h && sizeof(map) == w * h + 1);
    
    return map[i + j * w] == ' ';
    
}
