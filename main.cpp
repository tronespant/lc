#include <iostream>
#include <sysinfoapi.h>
#include <vector>
#include <dir.h>
#include <tchar.h>
#include "windows.h"
#include "filesystem"
#include "fstream"

namespace fs=std::filesystem;
int count(int &a,int &b){
    return a+b;
}
int count(const int *a,const int *b){
    return *a+*b;
}
bool endsWith(const std::string & str, const std::string & suffix){
    if (str.length()>=suffix.length()){
        return (0==str.compare(str.length()-suffix.length(),suffix.length(),suffix));
    }
    return false;
}
std::vector<std::string > stringList;
std::vector<std::string> pdf_png_list;
void listFileRecurssively(const fs::path& path){
    try {
        for (auto it=fs::directory_iterator(path);it!=fs::directory_iterator();++it) {
           const fs::path& entryPath=it->path();
            const std::filesystem::file_status &status = fs::status(entryPath);
            if (fs::is_directory(entryPath)){
                std::cout << "Directory: " << entryPath.generic_string() << std::endl;
                if (fs::is_symlink(entryPath)){
                    fs::path target=fs::read_symlink(entryPath);
                    std::cout << "Symbolic link target: " << target.generic_string() << std::endl;
                        // 决定是否递归处理符号链接目标
                        listFileRecurssively(target);
                }else{
                    listFileRecurssively(entryPath);
                }
            }else{
                const std::string &filePath = entryPath.generic_string();
                for (const auto& suffix: stringList) {
                    if (endsWith(filePath,suffix)){
                        std::cout << "File: " << entryPath.generic_string() << std::endl;
                        pdf_png_list.push_back(filePath);
                    }
                }
//                std::cout << "File: " << entryPath.generic_string() << std::endl;
            }
        }
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        std::cerr << "Path: " << path.generic_string() << std::endl;
        std::cerr << "Error code: " << ex.code().value() << " (" << ex.code().message() << ")" << std::endl;
    }
}

int getDiskInfo(){
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
//    printf("处理器个数 %lu",sysInfo.dwNumberOfProcessors);
//    printf("处理器类型 %lu",sysInfo.dwProcessorType);
    // 获取所有逻辑驱动器字符串的最大长度
    DWORD dwDriveStrLen;
    TCHAR wDrivesName[0x100];
    int DType;
    dwDriveStrLen = sizeof(wDrivesName);
    GetLogicalDriveStrings(dwDriveStrLen, wDrivesName);
    auto *p = (TCHAR *)wDrivesName;

    while (*p) {
        std::basic_string<TCHAR> s(p);
        fs::path path=p;
        listFileRecurssively(path);
        printf("%ls \n",path.c_str());
        DType = GetDriveType(p);
        printf("%s \n",s.c_str());
        p += _tcslen(p) + 1;
    }
    return 0;
}
int main() {
    stringList.emplace_back(".pdf");
    stringList.emplace_back(".png");

//    system("chcp 65001");
    getDiskInfo();

    std::cout<<"共找到文件: "<<pdf_png_list.size()<<std::endl;
    std::fstream f;
    f.open("data.txt",std::ios::out| std::ios::app);
    for(const auto& pdf:pdf_png_list){
        f<<pdf<<std::endl;
    }
    f.close();
//    int a=0;
//    int b=4;
//    int c=43;
//    int d=21;
//    int* cpoint=&c;
//    int *dpoint=&d;
//
//    std::cout << "Hello, World! "<< count(a,b) << std::endl;
//
//    std::cout << "Hello, World! "<< count(cpoint,dpoint) << std::endl;
    return 0;
}

