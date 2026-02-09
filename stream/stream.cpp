/*
   Stream: 用于处理输入输出的抽象概念
  e.g. std::cin.cout,cerr,（ifstream,ofstream,fstream）,（std::stringstream,istringstream,ostringstream）
                     //错误流            文件流                      字符串流

   //iostream, iomanip
       std::fixed <<std::setprecision(num); 固定小数点，保留到小数点后num位
       std::setw(num) 设置输出宽度
       std::left 左对齐

   //fstream 文件流；
   std::ofstream outfile("output.txt");
   if (!outfile)
   {
      std::cerr << "open failed\n";
      return 1;
   }
   outfile << "hello\nworld\n";
   outfile << 3.1415;
   outfile.close();

   std::ifstream inFile("output.txt");
   if (!inFile)
   {
      std::cerr << "open failed\n";
      return 1;
   }
   std::string str;
   while (std::getline(inFile, str))
   {
      std::cout << str << std::endl;
   }
   inFile.close();

     std::fstream File("data.txt", std::ios::in | std::ios::out | std::ios::app);
   if (!File)
   {
      std::cerr << "open failed\n";
      return 1;
   }
   File << "hello";
   File.seekg(0, std::ios::beg);
   std::string s;
   while (std::getline(File, s))
   {
      std::cout << s << std::endl;
   }
   File.close();

   int num = 23;
   std::string str = std::to_string(23);
   int d = std::stoi(str);
   int id = 10;
   std::string name = "ALic";
   double score = 95.3;
   std::stringstream ss;
   ss << name << score << id;
   std::string sctur = ss.str();
   std::cout << sctur;
*/
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
int main()
{
   
   return 0;
}