#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <cctype> 
#include <sstream>

using namespace std;

struct JsonNode {
    string tag, value;
    vector<JsonNode> children;

    string toJson(int indent = 0) const {
        string padding(indent, ' '), result;
        if (!tag.empty()) result += padding + "\"" + tag + "\": ";
        if (!children.empty()) {
            result += "{\n";
            for (size_t i = 0; i < children.size(); ++i) {
                result += children[i].toJson(indent + 2);
                if (i < children.size() - 1) result += ",";
                result += "\n";
            }
            result += padding + "}";
        } else {
            result += "\"" + value + "\"";
        }
        return result;
    }
};

// Converts XML to JSON
class XmlToJsonConverter {
public:
    string convertToJson(const string& xml) {
        stack<JsonNode> nodes;
        JsonNode root;
        string tag, value;
        bool insideTag = false, closingTag = false;

        for (char ch : xml) {
            if (ch == '<') {
                if (!value.empty()) {
                    trim(value);
                    if (!value.empty() && !nodes.empty()) nodes.top().value = value;
                    value.clear();
                }
                insideTag = true;
                closingTag = false;
            } else if (ch == '>') {
                insideTag = false;
                if (closingTag) {
                    JsonNode completed = nodes.top();
                    nodes.pop();
                    if (!nodes.empty()) nodes.top().children.push_back(completed);
                    else root = completed;
                } else {
                    nodes.push(JsonNode{tag});
                }
                tag.clear();
            } else if (insideTag) {
                if (ch == '/') closingTag = true;
                else tag += ch;
            } else {
                value += ch;
            }
        }
        return "{\n" + root.toJson(2) + "\n}";
    }

    void saveToFile(const string& json, const string& filename) {
        ofstream file(filename);
        if (file) file << json;
        else cerr << "Error: Could not open file for writing." << endl;
    }

private:
    void trim(string& str) {
        size_t start = 0, end = str.size();
        while (start < end && isspace(str[start])) ++start;
        while (end > start && isspace(str[end - 1])) --end;
        str = str.substr(start, end - start);
    }
};

string readXMLFile(string fileName)
{
    ifstream file(fileName);
    if (!file.is_open())
    {
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();

    string content = buffer.str();

    string compressedContent;
    for (char ch : content)
    {
        if (!isspace(ch))
        {
            compressedContent += ch;
        }
    }

    return compressedContent;
}

int main() {
    string fileName = "sample.xml";
    string xml = readXMLFile(fileName);

    if (xml.empty())
    {
        cout << "Error reading the XML file." << endl;
        return 1;
    }
    cout << "number of characters = " << xml.length() << endl;

    XmlToJsonConverter converter;
    string json = converter.convertToJson(xml);

    cout << "Converted JSON:\n" << json << endl;
    converter.saveToFile(json, "output.json");

    return 0;
}
