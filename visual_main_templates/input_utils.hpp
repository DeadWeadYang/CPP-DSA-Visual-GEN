#pragma once

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace DSA::VisualTemplates
{
    inline std::string read_file(const std::string &path)
    {
        std::ifstream in(path);
        if (!in)
            throw std::runtime_error("cannot open input file: " + path);
        std::ostringstream oss;
        oss << in.rdbuf();
        return oss.str();
    }

    inline std::string json_string_value(const std::string &json, const std::string &key, const std::string &fallback = "")
    {
        std::string needle = "\"" + key + "\"";
        auto pos = json.find(needle);
        if (pos == std::string::npos)
            return fallback;
        pos = json.find(':', pos + needle.size());
        if (pos == std::string::npos)
            return fallback;
        pos = json.find('"', pos + 1);
        if (pos == std::string::npos)
            return fallback;
        std::string out;
        bool escape = false;
        for (std::size_t i = pos + 1; i < json.size(); ++i)
        {
            char ch = json[i];
            if (escape)
            {
                if (ch == 'n')
                    out.push_back('\n');
                else if (ch == 't')
                    out.push_back('\t');
                else
                    out.push_back(ch);
                escape = false;
            }
            else if (ch == '\\')
                escape = true;
            else if (ch == '"')
                return out;
            else
                out.push_back(ch);
        }
        return fallback;
    }

    inline int json_int_value(const std::string &json, const std::string &key, int fallback = 0)
    {
        std::string needle = "\"" + key + "\"";
        auto pos = json.find(needle);
        if (pos == std::string::npos)
            return fallback;
        pos = json.find(':', pos + needle.size());
        if (pos == std::string::npos)
            return fallback;
        ++pos;
        while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos])))
            ++pos;
        if (pos < json.size() && json[pos] == '"')
        {
            std::string value = json_string_value(json, key, "");
            if (value.empty())
                return fallback;
            return std::stoi(value);
        }
        std::size_t end = pos;
        if (end < json.size() && (json[end] == '-' || json[end] == '+'))
            ++end;
        while (end < json.size() && std::isdigit(static_cast<unsigned char>(json[end])))
            ++end;
        if (end == pos)
            return fallback;
        return std::stoi(json.substr(pos, end - pos));
    }

    inline bool json_bool_value(const std::string &json, const std::string &key, bool fallback = false)
    {
        std::string needle = "\"" + key + "\"";
        auto pos = json.find(needle);
        if (pos == std::string::npos)
            return fallback;
        pos = json.find(':', pos + needle.size());
        if (pos == std::string::npos)
            return fallback;
        ++pos;
        while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos])))
            ++pos;
        if (pos < json.size() && json[pos] == '"')
        {
            std::string value = json_string_value(json, key, "");
            return value == "true" || value == "1" || value == "yes" || value == "on";
        }
        if (json.compare(pos, 4, "true") == 0)
            return true;
        if (json.compare(pos, 5, "false") == 0)
            return false;
        return fallback;
    }

    inline std::vector<int> parse_int_list(std::string text)
    {
        for (char &ch : text)
            if (ch == ',' || ch == ';' || ch == '[' || ch == ']')
                ch = ' ';
        std::istringstream iss(text);
        std::vector<int> values;
        int x = 0;
        while (iss >> x)
            values.push_back(x);
        return values;
    }

    inline std::vector<std::pair<std::string, int>> parse_value_ops(std::string text)
    {
        for (char &ch : text)
            if (ch == ',' || ch == ';')
                ch = ' ';
        std::istringstream iss(text);
        std::vector<std::pair<std::string, int>> ops;
        std::string op;
        int x = 0;
        while (iss >> op)
        {
            if (op == "pop")
            {
                ops.push_back({op, 0});
                continue;
            }
            if (!(iss >> x))
                break;
            ops.push_back({op, x});
        }
        return ops;
    }

    inline std::vector<std::tuple<std::string, int, int>> parse_dsu_ops(std::string text)
    {
        for (char &ch : text)
            if (ch == ',' || ch == ';')
                ch = ' ';
        std::istringstream iss(text);
        std::vector<std::tuple<std::string, int, int>> ops;
        std::string op;
        while (iss >> op)
        {
            int a = 0, b = 0;
            if (op == "find")
            {
                if (!(iss >> a))
                    break;
                ops.push_back({op, a, 0});
            }
            else
            {
                if (!(iss >> a >> b))
                    break;
                ops.push_back({op, a, b});
            }
        }
        return ops;
    }

    inline std::vector<std::pair<int, int>> parse_edges(std::string text)
    {
        for (char &ch : text)
            if (ch == ',' || ch == ';')
                ch = ' ';
        std::istringstream iss(text);
        std::vector<std::pair<int, int>> edges;
        int u = 0, v = 0;
        while (iss >> u >> v)
            edges.push_back({u, v});
        return edges;
    }

    inline std::vector<std::tuple<int, int, int>> parse_weighted_edges(std::string text)
    {
        for (char &ch : text)
            if (ch == ',' || ch == ';')
                ch = ' ';
        std::istringstream iss(text);
        std::vector<std::tuple<int, int, int>> edges;
        int u = 0, v = 0, w = 0;
        while (iss >> u >> v >> w)
            edges.push_back({u, v, w});
        return edges;
    }

    inline std::string arg_or_default(int argc, char **argv, int index, const std::string &fallback)
    {
        return argc > index ? std::string(argv[index]) : fallback;
    }
}
