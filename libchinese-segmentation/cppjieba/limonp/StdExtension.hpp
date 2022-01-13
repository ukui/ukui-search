/*
 * Copyright (C) 2022, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: yanyiwu
 * Modified by: jixiaoxu <jixiaoxu@kylinos.cn>
 *
 */
#ifndef LIMONP_STD_EXTEMSION_HPP
#define LIMONP_STD_EXTEMSION_HPP

#include <map>

#ifdef __APPLE__
#include <unordered_map>
#include <unordered_set>
#elif(__cplusplus >= 201103L)
#include <unordered_map>
#include <unordered_set>
#elif defined _MSC_VER
#include <unordered_map>
#include <unordered_set>
#else
#include <tr1/unordered_map>
#include <tr1/unordered_set>
namespace std {
using std::tr1::unordered_map;
using std::tr1::unordered_set;
}

#endif

#include <set>
#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>

namespace std {

template<typename T>
ostream& operator << (ostream& os, const vector<T>& v) {
  if(v.empty()) {
    return os << "[]";
  }
  os<<"["<<v[0];
  for(size_t i = 1; i < v.size(); i++) {
    os<<", "<<v[i];
  }
  os<<"]";
  return os;
}

template<>
inline ostream& operator << (ostream& os, const vector<string>& v) {
  if(v.empty()) {
    return os << "[]";
  }
  os<<"[\""<<v[0];
  for(size_t i = 1; i < v.size(); i++) {
    os<<"\", \""<<v[i];
  }
  os<<"\"]";
  return os;
}

template<typename T>
ostream& operator << (ostream& os, const deque<T>& dq) {
  if(dq.empty()) {
    return os << "[]";
  }
  os<<"[\""<<dq[0];
  for(size_t i = 1; i < dq.size(); i++) {
    os<<"\", \""<<dq[i];
  }
  os<<"\"]";
  return os;
}


template<class T1, class T2>
ostream& operator << (ostream& os, const pair<T1, T2>& pr) {
  os << pr.first << ":" << pr.second ;
  return os;
}


template<class T>
string& operator << (string& str, const T& obj) {
  stringstream ss;
  ss << obj; // call ostream& operator << (ostream& os,
  return str = ss.str();
}

template<class T1, class T2>
ostream& operator << (ostream& os, const map<T1, T2>& mp) {
  if(mp.empty()) {
    os<<"{}";
    return os;
  }
  os<<'{';
  typename map<T1, T2>::const_iterator it = mp.begin();
  os<<*it;
  it++;
  while(it != mp.end()) {
    os<<", "<<*it;
    it++;
  }
  os<<'}';
  return os;
}
template<class T1, class T2>
ostream& operator << (ostream& os, const std::unordered_map<T1, T2>& mp) {
  if(mp.empty()) {
    return os << "{}";
  }
  os<<'{';
  typename std::unordered_map<T1, T2>::const_iterator it = mp.begin();
  os<<*it;
  it++;
  while(it != mp.end()) {
    os<<", "<<*it++;
  }
  return os<<'}';
}

template<class T>
ostream& operator << (ostream& os, const set<T>& st) {
  if(st.empty()) {
    os << "{}";
    return os;
  }
  os<<'{';
  typename set<T>::const_iterator it = st.begin();
  os<<*it;
  it++;
  while(it != st.end()) {
    os<<", "<<*it;
    it++;
  }
  os<<'}';
  return os;
}

template<class KeyType, class ContainType>
bool IsIn(const ContainType& contain, const KeyType& key) {
  return contain.end() != contain.find(key);
}

template<class T>
basic_string<T> & operator << (basic_string<T> & s, ifstream & ifs) {
  return s.assign((istreambuf_iterator<T>(ifs)), istreambuf_iterator<T>());
}

template<class T>
ofstream & operator << (ofstream & ofs, const basic_string<T>& s) {
  ostreambuf_iterator<T> itr (ofs);
  copy(s.begin(), s.end(), itr);
  return ofs;
}

} // namespace std

#endif
