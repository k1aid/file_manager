#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include <clocale>

using namespace std;

class FileSystemItem {
protected:
    string path;
public:
    FileSystemItem(string p) : path(p) {}
    virtual ~FileSystemItem() = default;
    virtual unsigned long long getSize() const = 0;
};

class FileItem : public FileSystemItem {
public:
    using FileSystemItem::FileSystemItem;
    unsigned long long getSize() const override {
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &fad)) {
            LARGE_INTEGER size;
            size.LowPart = fad.nFileSizeLow;
            size.HighPart = fad.nFileSizeHigh;
            return size.QuadPart;
        }
        return 0;
    }
};

class DirectoryItem : public FileSystemItem {
public:
    using FileSystemItem::FileSystemItem;
    unsigned long long getSize() const override { return calculateSize(path); }
private:
    unsigned long long calculateSize(string folderPath) const {
        unsigned long long totalSize = 0;
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA((folderPath + "\\*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) return 0;
        do {
            string name = fd.cFileName;
            if (name != "." && name != "..") {
                string fullPath = folderPath + "\\" + name;
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    totalSize += calculateSize(fullPath);
                else {
                    LARGE_INTEGER li; li.LowPart = fd.nFileSizeLow; li.HighPart = fd.nFileSizeHigh;
                    totalSize += li.QuadPart;
                }
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
        return totalSize;
    }
};