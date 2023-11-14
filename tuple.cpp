#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <iomanip>
#include <tuple>
#include <algorithm>
// Type to store the data from the CSV file
using TupleType = std::tuple<std::string, std::string, char>;
// class to read Bin, csv and txt files
class FileReader
{
private:
 std::string filename;
 std::vector<char> rawData;
public:
 FileReader(const std::string &filename) : filename(filename)
 {
 // Read the file into rawData
 std::ifstream file(filename, std::ios::binary);
 if (file)
 {
 file.seekg(0, std::ios::end);
 std::streamsize size = file.tellg();
 file.seekg(0, std::ios::beg);
 if (size > 0)
 {
 rawData.resize(static_cast<size_t>(size));
 file.read(&rawData[0], size);
 }
 file.close();
 }
 }
 std::streamsize Size() const
 {
 return rawData.size();
 }
 std::string Name() const
 {
 return filename;
 }
 const std::vector<char> &Raw() const
 {
 return rawData;
 }
 void Parse(std::function<void(const std::vector<char> &)> callback)
 {
 callback(rawData);
 }
};
// Callback function for CSV parsing and storing data in a vector of tuples
void ParseCSV(const std::vector<char> &rawData, std::vector<TupleType>
&tupleVector)
{
 std::istringstream stream(std::string(rawData.begin(), rawData.end()));
 std::string line;
 while (std::getline(stream, line))
 {
 std::istringstream lineStream(line);
 std::string code, column;
 char ascii;
 if (std::getline(lineStream, code, ',') &&
 std::getline(lineStream, column, ',') &&
 lineStream >> ascii)
 {
 tupleVector.emplace_back(code, column, ascii);
 }
 }
}
// Search function using std::find and a lambda expression
auto SearchTuple(const std::vector<TupleType> &tupleVector, const std::string
&searchField, const std::string &searchValue)
{
 return std::find_if(tupleVector.begin(), tupleVector.end(), [&](const TupleType
&tuple)
 {
 if (searchField == "code")
 return std::get<0>(tuple) == searchValue;
 else if (searchField == "column")
 return std::get<1>(tuple) == searchValue;
 else if (searchField == "ascii")
 return std::get<2>(tuple) == searchValue[0];
 else
 return false; });
}
// Unit test for CSV parsing and searching
void TestCSVReader(const std::string &filename)
{
 FileReader reader(filename);
 std::vector<TupleType> tupleVector;
 reader.Parse([&](const std::vector<char> &data)
 { ParseCSV(data, tupleVector); });
 // Search for a specific tuple
 std::string searchField = "code";
 std::string searchValue = "Y3";
 auto result = SearchTuple(tupleVector, searchField, searchValue);
 if (result != tupleVector.end())
 {
 std::cout << "Found Tuple by code: (" << std::get<0>(*result) << ", " <<
std::get<1>(*result) << ", " << std::get<2>(*result) << ")\n";
 }
 else
 {
 std::cout << "Tuple not found.\n";
 }
 searchField = "column";
 searchValue = "3";
 result = SearchTuple(tupleVector, searchField, searchValue);
 if (result != tupleVector.end())
 {
 std::cout << "Found Tuple by column: (" << std::get<0>(*result) << ", " <<
std::get<1>(*result) << ", " << std::get<2>(*result) << ")\n";
 }
 else
 {
 std::cout << "Tuple not found.\n";
 }
 searchField = "ascii";
 searchValue = "!";
 result = SearchTuple(tupleVector, searchField, searchValue);
 if (result != tupleVector.end())
 {
 std::cout << "Found Tuple by ascii: (" << std::get<0>(*result) << ", " <<
std::get<1>(*result) << ", " << std::get<2>(*result) << ")\n";
 }
 else
 {
 std::cout << "Tuple not found.\n";
 }
}
int main()
{
 std::string csvFilename = "Tuple.csv";
 // Test CSV parsing and searching
 TestCSVReader(csvFilename);
 return 0;
}