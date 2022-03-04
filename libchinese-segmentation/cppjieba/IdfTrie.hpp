#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <cmath>
#include <limits>
#include "limonp/StringUtil.hpp"
#include "limonp/Logging.hpp"
#include "Unicode.hpp"
#include "DatTrie.hpp"
#include <QDebug>
namespace cppjieba {

using namespace limonp;

const size_t IDF_COLUMN_NUM = 2;

class IdfTrie {
public:
    enum UserWordWeightOption {
        WordWeightMin,
        WordWeightMedian,
        WordWeightMax,
    }; // enum UserWordWeightOption

    IdfTrie(const string& dict_path, const string & dat_cache_path = "",
             UserWordWeightOption user_word_weight_opt = WordWeightMedian) {
        Init(dict_path, dat_cache_path, user_word_weight_opt);
    }

    ~IdfTrie() {}

    double Find(const string & word, std::size_t length = 0, std::size_t node_pos = 0) const {
        return dat_.Find(word, length, node_pos);
    }

    size_t GetTotalDictSize() const {
        return total_dict_size_;
    }

private:
    void Init(const string& dict_path, string dat_cache_path,
              UserWordWeightOption user_word_weight_opt) {
        size_t file_size_sum = 0;
        const string md5 = CalcFileListMD5(dict_path, file_size_sum);
        total_dict_size_ = file_size_sum;

        if (dat_cache_path.empty()) {
            //未指定词库数据文件存储位置的默认存储在tmp目录下--jxx20200519
            dat_cache_path = /*dict_path*/"/tmp/" + md5 + "." + to_string(user_word_weight_opt) +  ".dat_cache";
        }
        QString path = QString::fromStdString(dat_cache_path);
        qDebug() << "#########Idf path:" << path;
        if (dat_.InitIdfAttachDat(dat_cache_path, md5)) {
            return;
        }

        LoadDefaultIdf(dict_path);
        double idf_sum_ = CalcIdfSum(static_node_infos_);
        assert(static_node_infos_.size());
        idfAverage_ = idf_sum_ / static_node_infos_.size();
        assert(idfAverage_ > 0.0);
        double min_weight = 0;
        dat_.SetMinWeight(min_weight);

        const auto build_ret = dat_.InitBuildDat(static_node_infos_, dat_cache_path, md5);
        assert(build_ret);
        vector<IdfElement>().swap(static_node_infos_);
    }

    void LoadDefaultIdf(const string& filePath) {
        ifstream ifs(filePath.c_str());
        if(not ifs.is_open()){
            return ;
        }
        XCHECK(ifs.is_open()) << "open " << filePath << " failed.";
        string line;
        vector<string> buf;
        size_t lineno = 0;

        for (; getline(ifs, line); lineno++) {
            if (line.empty()) {
                XLOG(ERROR) << "lineno: " << lineno << " empty. skipped.";
                continue;
            }
            Split(line, buf, " ");
            XCHECK(buf.size() == IDF_COLUMN_NUM) << "split result illegal, line:" << line;
            IdfElement node_info;
            node_info.word = buf[0];
            node_info.idf = atof(buf[1].c_str());
            static_node_infos_.push_back(node_info);
        }
    }

    double CalcIdfSum(const vector<IdfElement>& node_infos) const {
        double sum = 0.0;

        for (size_t i = 0; i < node_infos.size(); i++) {
            sum += node_infos[i].idf;
        }

        return sum;
    }
public:
    double idfAverage_;
private:
    vector<IdfElement> static_node_infos_;
    size_t total_dict_size_ = 0;
    DatTrie dat_;
};
}

