// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dirent.h>
#endif

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;

struct RawImage {
  std::vector<u8> data;
  i32 width, height;
};

std::vector<std::string> GetFiles(const std::string& path)
{
  std::vector<std::string> files;
#ifdef _WIN32
  WIN32_FIND_DATA data;

  for (auto* handle = FindFirstFile((path + "\\*.bmp").c_str(), &data);
       FindNextFile(handle, &data);) {
    if (handle == INVALID_HANDLE_VALUE) {
      std::cerr << "Bad handle" << std::endl;
      return {};
    }

    std::cout << "Loop." << std::endl;
    if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || true)
      files.push_back(data.cFileName);
  }
#else
  auto* dir = opendir(path.c_str());
  if (dir == nullptr)
    return {};

  for (auto* entry = readdir(dir); entry != nullptr; entry = readdir(dir)) {
    const std::string name = std::string(entry->d_name);
    if (entry->d_type == DT_REG && name.substr(name.length() - 3) == "bmp")
      files.push_back(entry->d_name);
  }

  closedir(dir);
#endif
  return files;
}

// Terrible homebrew BMP parser
RawImage ReadBMP(const std::string& path)
{
  std::ifstream ifs(path);

  if (!ifs.good()) {
    std::cerr << path << ": Failed to open file for reading" << std::endl;
    return {};
  }

  // Check for bitmap magic
  if (ifs.get() != 'B' || ifs.get() != 'M') {
    std::cerr << path << ": Invalid bitmap" << std::endl;
    return {};
  }

  ifs.seekg(0x1E);

  if (ifs.get() != 0) {
    std::cerr << path << ": Bitmap not uncompressed RGB" << std::endl;
    return {};
  }

  RawImage image;

  ifs.seekg(0x12);
  ifs.read(reinterpret_cast<char*>(&image.width), 4);
  ifs.seekg(0x16);
  ifs.read(reinterpret_cast<char*>(&image.height), 4);

  u32 size;
  u32 offset;

  ifs.seekg(0x02);
  ifs.read(reinterpret_cast<char*>(&size), 4);

  ifs.seekg(0x0A);
  ifs.read(reinterpret_cast<char*>(&offset), 4);

  if (!ifs.good()) {
    std::cerr << path << ": Failed to read header" << std::endl;
    return {};
  }

  ifs.seekg(offset);

  image.data.resize(size);

  ifs.read(reinterpret_cast<char*>(image.data.data()), size);

  return image;
}

RawImage CreateDiff(const RawImage& a, const RawImage& b)
{
  RawImage diff = b;

  for (size_t i = 0; i < diff.data.size(); i++) {
    if (i < a.data.size())
      diff.data[i] -= a.data[i];
  }

  return diff;
}

bool IsZeroDiff(const RawImage& diff)
{
  for (size_t i = 0; i < diff.data.size(); i++) {
    if (diff.data[i] != 0x00)
      return false;
  }
  return true;
}

template <typename T> void WriteData(std::ofstream& stream, T data)
{
  stream.write(reinterpret_cast<char*>(&data), sizeof(T));
}

bool WriteBMP(const RawImage& image, const std::string& path)
{
  std::ofstream ofs(path, std::ios::binary);

  if (!ofs.good()) {
    std::cerr << path << ": Failed to open file for writing" << std::endl;
    return false;
  }

  u32 size = 0x36 + static_cast<u32>(image.data.size());

  // Magic
  ofs << "BM";
  // File size
  WriteData(ofs, size);
  // Unused
  WriteData<u32>(ofs, 0);
  // Offset
  WriteData<u32>(ofs, 0x36);

  // DIB header

  WriteData<u32>(ofs, 40);
  WriteData<u32>(ofs, image.width);
  WriteData<u32>(ofs, image.height);
  WriteData<u16>(ofs, 1);
  WriteData<u16>(ofs, 24);
  WriteData<u32>(ofs, 0);
  WriteData<u32>(ofs, static_cast<u32>(image.data.size()));
  WriteData<u32>(ofs, image.width);
  WriteData<u32>(ofs, image.height);
  WriteData<u64>(ofs, 0);

  ofs.write(reinterpret_cast<const char*>(image.data.data()),
            image.data.size());

  return true;
}

int main(int argc, char** argv)
{

  if (argc != 4) {
    std::cerr << argv[0] << " [summary/html] [folderA] [folderB]" << std::endl;
    return 1;
  }

  std::string mode = argv[1];

  if (mode != "html" && mode != "summary") {
    std::cerr << "Bad format provided (Must be either summary or html)"
              << std::endl;
    return 1;
  }

  std::string dirA = argv[2];
  std::string dirB = argv[3];

  if (dirA == dirB) {
    std::cerr << "Identical folders specified." << std::endl;
    return -1;
  }

  std::vector<std::string> filesA, filesB;

  filesA = GetFiles(dirA);
  filesB = GetFiles(dirB);

  if (filesA.empty() || filesB.empty()) {
    std::cerr << "One of the supplied directories is invalid or empty!"
              << std::endl;
    return -2;
  }

  std::vector<std::string> deletions{}, additions{}, changes{};

  // TODO: Do the actual comparison
  for (const auto& file : filesA) {
    if (std::find(filesB.begin(), filesB.end(), file) == filesB.end())
      deletions.push_back(file);
  }

  for (const auto& file : filesB) {
    if (std::find(filesA.begin(), filesA.end(), file) == filesA.end())
      additions.push_back(file);
  }

  for (const auto& file : filesA) {
    if (std::find(filesB.begin(), filesB.end(), file) != filesB.end()) {
      RawImage a = ReadBMP(dirA + "/" + file);
      RawImage b = ReadBMP(dirB + "/" + file);

      if (a.data.size() == 0)
        return -2;

      if (b.data.size() == 0)
        return -2;

      std::cout << "Comparing " << file << std::endl;
      std::cout << "A: " << a.width << "x" << a.height << " " << a.data.size()
                << std::endl;
      std::cout << "B: " << b.width << "x" << b.height << " " << b.data.size()
                << std::endl;

      RawImage diff = CreateDiff(a, b);

      std::cout << "DIFF: " << b.width << "x" << b.height << " "
                << b.data.size() << std::endl;

      if (IsZeroDiff(diff))
        continue;

      WriteBMP(diff, file + ".dff.bmp");
      changes.push_back(file);
    }
  }

  std::cout << std::endl;

  // Handle Output
  for (const auto& file : deletions) {
    if (mode == "summary") {
      std::cout << "REMOVED " << file << std::endl;
    } else {
      // TODO: HTML
    }
  }

  for (const auto& file : additions) {
    if (mode == "summary") {
      std::cout << "ADDED " << file << std::endl;
    } else {
      // TODO: HTML
    }
  }

  for (const auto& file : changes) {
    if (mode == "summary") {
      std::cout << "CHANGED " << file << std::endl;
    } else {
      // TODO: HTML
    }
  }

  // This short verdict will be printed regardless of output mode
  auto change_count = deletions.size() + additions.size() + changes.size();
  std::cout << std::endl;
  if (change_count == 0) {
    std::cout << "No changes detected." << std::endl;
    return 0;
  } else {
    std::cout << change_count << " change(s) detected." << std::endl;
  }

  return 0;
}
